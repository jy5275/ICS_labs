#include <stdio.h>
#include "csapp.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";

void *doit(void *fd);
void read_requesthdrs(rio_t *rp, char *host, char *header);
void parse_url(char *url, char *host, char *port, char *filename);

int main(int argc, char **argv) {
	signal(SIGPIPE, SIG_IGN);
    int listenfd, *connfdp;
    pthread_t tid;
    char hostname[MAXLINE], port[MAXLINE];
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
	
    if (argc != 2) {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(1);
    }

    listenfd = Open_listenfd(argv[1]);
    //printf("Open-%d(listen)\r\n", listenfd);
    while (1) {
		clientlen = sizeof(clientaddr);
		connfdp = (int*)Malloc(sizeof(int));
		*connfdp = Accept(listenfd, (SA*)&clientaddr, &clientlen);		// A request from clientaddr arrives!
		printf("Open-%d.\r\n", *connfdp);
		//Getnameinfo((SA*)&clientaddr, clientlen, hostname, MAXLINE, port,MAXLINE,0);
        //printf("Accepted connection from (%s %s).\n", hostname, port);

		Pthread_create(&tid, NULL, doit, connfdp);			// Fetch web page on behalf of connfd
    }
}


/* doit - send web request to host and fetch html content */
void *doit(void *connfdp) {
    int clientfd, connfd = *(int*)connfdp;
    Free(connfdp);
    Pthread_detach(pthread_self());
    char buf[MAXLINE], method[MAXLINE], url[MAXLINE], version[MAXLINE];
    char filename[MAXLINE], port[MAXLINE], host[MAXLINE], header[MAXLINE];
    rio_t rio_client, rio_host;
    Rio_readinitb(&rio_client, connfd);
    Rio_readlineb(&rio_client, buf, MAXLINE);
    sscanf(buf, "%s %s %s", method, url, version);
    
    if (strcasecmp(method, "GET")) {
        fprintf(stderr, "Tiny does not implement this method: %s\r\n", method);
        return NULL;
    }
    
    parse_url(url, host, port, filename);
    //printf("[Check parse_url]  host=%s   port=%s    filename=%s\r\n", host, port, filename);
    read_requesthdrs(&rio_client, host, header);
    printf("[Check parse_url]  host=%s   port=%s   filename=%s\r\n", host, port, filename);
    
    sprintf(buf, "GET %s HTTP/1.0\r\n", filename);
    char tot[MAXLINE];
    sprintf(tot, "%s%s", buf, header);
	//printf("\r\n[------CHECK-------]\r\n%s[------END OF CHECK-------]\r\n\r\n", tot);
    
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
    while ((nread = Rio_readlineb(&rio_host, buf, MAXLINE)) > 0) {
    	printf("Received %ld bytes from target server.\r\n", nread);
        rio_writen(connfd, buf, nread);
    }
    printf("Receive EOF. Close connect!\r\n");
    Close(clientfd);
    Close(connfd);
    printf("Closed-%d.\r\n", clientfd);
    return NULL;
}


/* Read request header from rio-buffer rp (file connfd), header get ready */
void read_requesthdrs(rio_t *rp, char *host, char *header) {
    char buf[MAXLINE], *p, *phost, header_o[MAXLINE] = {'\0'}, host_hdr[MAXLINE];
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
