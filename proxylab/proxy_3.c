#include <stdio.h>
#include "csapp.h"
#include "sbuf.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";
int readcnt, ntime;
sem_t wlock, rmutex;
typedef struct{
	sem_t rmutex, wlock;
	int rank;
	int valid, readcnt, n;
	char content[MAX_OBJECT_SIZE];
	char url[MAXLINE];
}cache_t;
cache_t block[10];

void *doit(void *fd);
void read_requesthdrs(rio_t *rp, char *host, char *header);
void parse_url(char *url, char *host, char *port, char *filename);

void Init();
int Search(char *url);
void Fetch(int connfd, int index);
int FindBlock();
void Insert(char *url, char *content, int nmain);
int Search(char *url);

void Init() {
	Sem_init(&rmutex, 0, 1);
	Sem_init(&wlock, 0, 1);
	readcnt = 0;
	int i;
	for (i = 0; i < 10; i++) {
		Sem_init(&block[i].rmutex, 0, 1);
		Sem_init(&block[i].wlock, 0, 1);
		block[i].valid = 0;
		block[i].readcnt = 0;
	}
}

int Search(char *url) {	// return char* if url is cached
	//printf("Search::begin, readcnt=%d\r\n", readcnt);
	int ans = -1;
	P(&rmutex);
	readcnt++;
	if (readcnt == 1)
		P(&wlock);	// Can't write cache block while wlock is set
	V(&rmutex);
	int i;
	for (i = 0; i < 10; i++) {
		if (!block[i].valid) continue;
		if (strcmp(url, block[i].url) == 0) {
			ans = i;
			printf("Find url: %s\r\n", url);
			P(&block[i].rmutex);	// Inc b[i].readcnt
			block[i].readcnt++;
			printf("Locked block[i].rmutex\r\n");
			if (block[i].readcnt == 1)
				P(&block[i].wlock);	// Lock this block
			
			printf("Locked block[i].wlock\r\n");
			V(&block[i].rmutex);
			printf("Unlocked block[i].rmutex\r\n");
			break;
		}
	}
	//printf("Search::find ans, ans=%d\r\n", ans);
	P(&rmutex);
	readcnt--;
	if (readcnt == 0)
		V(&wlock);	// Release wlock if none is reading
	V(&rmutex);
	
	return ans;
}

void Fetch(int connfd, int index){	// Send cached content back to client
	//printf("Fetch::begin\r\n\r\n");
	char header[MAXLINE];
	sprintf(header, "HTTP/1.0 200 OK\r\nServer: Tiny Web Server\r\n");
	sprintf(header, "%sConnection: close\r\nContent-length: 6532\r\n", header);
	sprintf(header, "%sContent-type: text/plain\r\n\r\n", header);
	Rio_writen(connfd, header, strlen(header));
	Rio_writen(connfd, block[index].content, block[index].n);
	ntime++;
	block[index].rank = ntime;		// Update rank of block[index]
	
	P(&block[index].rmutex);
	block[index].readcnt--;
	if (block[index].readcnt == 0)
		V(&block[index].wlock);
	V(&block[index].rmutex);
}

int FindBlock() {	// 找到一个合适的block，加锁
	P(&rmutex);
	readcnt++;
	if (readcnt == 1)
		P(&wlock);
	V(&rmutex);
	int i, ans = -1, leastrank = 1 << 30, leasti = 0;
	for (i = 0; i < 10; i++) {
		if (block[i].rank < leastrank) {
			leastrank = block[i].rank;
			leasti = i;
		}
		if (!block[i].valid) {
			ans = i;
			break;
		}
	}
	if (ans == -1)		// No free block...find a victim!
		ans = leasti;
	// printf("FindBlock::Find block %d, readcnt=%d\r\n", ans, readcnt);
	// 如果填入空Block，返回时一定仍然为空，因为wlock阻塞了其他人写入该block
	// 如果victim，返回时不再是严格LRU，因为中间可能有人Fetch，从而更改rank
	P(&block[ans].rmutex);
	block[ans].readcnt--;
	if (block[ans].readcnt == 1)
		V(&block[ans].wlock);
	V(&block[ans].rmutex);
	
	P(&rmutex);
	readcnt--;
	if (readcnt == 0)
		V(&wlock);
	V(&rmutex);
	
	return ans;
}

void Insert(char *url, char *content, int nmain) {
	int index = FindBlock();
	//printf("Insert::FindBlock returned, index=%d\r\n", index);
	//等到block[index]读者全部退出，释放写锁，才能写这个block
	//其实不用管全局锁，都能进单独block了还怕全局乱了么？
	P(&block[index].wlock);
	//printf("Insert::write to cache begin\r\n");
	ntime++;
	block[index].rank = ntime;
	block[index].n = nmain;
	block[index].valid = 1;
	//printf("Insert::for this block, rank=%d, n=%d\r\n", ntime, nmain);
	sprintf(block[index].url, "%s", url);
	sprintf(block[index].content, "%s", content);
	//printf("Insert::in cache: url=%s", block[index].url);
	V(&block[index].wlock);
}


//GET http://localhost:18855/csapp.h HTTP/1.1
int main(int argc, char **argv) {
	signal(SIGPIPE, SIG_IGN);
    int listenfd, *connfdp;
    pthread_t tid;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
	
    if (argc != 2) {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(1);
    }
	Init();
    listenfd = Open_listenfd(argv[1]);
    while (1) {
		clientlen = sizeof(clientaddr);
		connfdp = (int*)Malloc(sizeof(int));
		*connfdp = Accept(listenfd, (SA*)&clientaddr, &clientlen);
		Pthread_create(&tid, NULL, doit, connfdp);
    }
}


/* doit - send web request to host and fetch html content */
void *doit(void *connfdp) {
    int clientfd, connfd = *(int*)connfdp;
    Free(connfdp);
    Pthread_detach(pthread_self());
    char buf[MAXLINE], method[MAXLINE], url[MAXLINE], version[MAXLINE];
    char filename[MAXLINE], port[MAXLINE], host[MAXLINE], header[MAXLINE];
    char cachebuf[MAX_OBJECT_SIZE], url_o[MAXLINE];
    rio_t rio_client, rio_host;
    Rio_readinitb(&rio_client, connfd);
    Rio_readlineb(&rio_client, buf, MAXLINE);
    sscanf(buf, "%s %s %s", method, url_o, version);
    
    if (strcasecmp(method, "GET")) {
        fprintf(stderr, "Tiny does not implement this method: %s\r\n",
        	method);
        Close(connfd);
        return NULL;
    }
    
    sprintf(url, "%s", url_o);
    parse_url(url, host, port, filename);
    read_requesthdrs(&rio_client, host, header);
    printf("[Check parse_url]  host=%s   port=%s   filename=%s\r\n", 
    	host, port, filename);
    
    int index = Search(url_o);
    //printf("doit::search end, index=%d\r\n", index);
    if (index != -1) {	// Find this url in cache!
    	Fetch(connfd, index);
    	Close(connfd);
    	return NULL;
    }
    
    //cache中没找到url才进行以下操作...
    sprintf(buf, "GET %s HTTP/1.0\r\n", filename);
    char tot[MAXLINE];
    sprintf(tot, "%s%s", buf, header);
       
    struct addrinfo hints, *listp;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICSERV|AI_ADDRCONFIG;
    if (getaddrinfo(host, port, &hints, &listp) < 0) {
    	perror("getaddrinfo error");
    	return NULL;
	}
    clientfd = open_clientfd(host, port);
    if (clientfd < 0) {
    	perror("open_clientfd error");
    	return NULL;
    }
    Rio_readinitb(&rio_host, clientfd);
    Rio_writen(clientfd, tot, strlen(tot));
	
    size_t nread;
    int entermain = 0, nmain = 0;
    while ((nread = Rio_readlineb(&rio_host, buf, MAXLINE)) > 0) {
    	//printf("strlen(buf)=%ld, entermain=%d\r\n", strlen(buf), entermain);
        rio_writen(connfd, buf, nread);
        
        if (entermain) {
        	nmain += nread;
        	if (nmain <= MAX_OBJECT_SIZE)
        		sprintf(cachebuf, "%s%s", cachebuf, buf);
		}
    	if (strlen(buf) <= 2)
    		entermain = 1;
        
    }
    printf("Receive EOF. Close connect. Main is %d byte-long.\r\n", nmain);
    Close(clientfd);
    Close(connfd);
    printf("Closed-%d.\r\n", clientfd);
    if (nmain <= MAX_OBJECT_SIZE && nmain >= 4) {
    	Insert(url_o, cachebuf, nmain);
    	printf("Cached!\r\n");
    }
    return NULL;
}


/* Read request header from rio-buffer rp (file connfd), header get ready */
void read_requesthdrs(rio_t *rp, char *host, char *header) {
    char buf[MAXLINE], *p, *phost, header_o[MAXLINE], host_hdr[MAXLINE];
    int headset = 0;
    while (Rio_readlineb(rp, buf, MAXLINE) > 0) {
    	if (strcmp(buf, "\r\n") == 0) break;
    	
    	if (strstr(buf, "Host:")) {		// This is real host!
    		p = strstr(buf, ":");
    		phost = p + 2;
    		sprintf(host_hdr, "%s", phost);
    		headset = 1;
    		continue;
    	}
		if (strstr(buf, "User-Agent") || strstr(buf, "Connection") || 
			strstr(buf, "Proxy-connection")) continue;
		strcat(header_o, buf);
    }
    
   	if (!headset) {			// Real host is already lying in req line...
   		sprintf(host_hdr, "%s\r\n", host);
   	}
   	
    sprintf(header, "Host: %s", host_hdr);
    sprintf(header, "%sConnection: close\r\n", header);
    sprintf(header, "%sProxy-Connection: close\r\n", header);
    sprintf(header, "%s%s", header, user_agent_hdr);
    sprintf(header, "%s%s", header, header_o);
	
    strcat(header, "\r\n");
    return;
}


//GET http://www.cmu.edu/hub/index.html HTTP/1.1
/* parse_url - parse URL into host, port and filename
 *             set default hostname if do not find Host field
 */
void parse_url(char *url, char *host, char *port, char *filename) {
	int port_num;
    char *p = strstr(url, "//");
    if (!p) p = url;	// No "http://" in received request line
    else p += 2;
    char *phost = p;	// First char of host in url
    
    p = strstr(phost, ":");
    if (p) {
    	*p = '\0';
    	sscanf(phost, "%s", host);
    	sscanf(p+1, "%d%s", &port_num, filename);
    }
    else {		// Port number not dictated in received request line
    	port_num = 80;
    	char *pdiv = strstr(phost, "/");
    	if (pdiv) {
    		*pdiv = '\0';
    		sscanf(phost, "%s", host);
    		*pdiv = '/';
    		sscanf(pdiv, "%s", filename);
    	}
    	else sscanf(phost, "%s", host);
    }
    sprintf(port, "%d", port_num);
}
