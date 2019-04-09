#include "csapp.h"

void handler1(int sig) {
	int olderrno = errno;
	while(waitpid(-1, NULL, 0) > 0)  // Keep reaping until there's no zombie child in {-1}.
		Sio_puts("Handler reaped child\n");
	if (errno != ECHILD)
		Sio_error("waitpid error");
	Sleep(1);
	errno = olderrno;
}

int main() {
	int n;
	char buf[MAXBUF];

	signal(SIGCHLD, handler1);
	
	/* Parent creates children who will soon die. */
	for (int i = 0; i < 3; i++) {
		if (Fork() == 0) {
			printf("Hello from child %d and exit...\n", (int)getpid());
			exit(0);
		}
	}
	
	/* Parent wait for terminal input and then process it. */
	printf("Parent processing input\n");
	while(1) {
		
		if ((n = read(STDIN_FILENO, buf, sizeof(buf))) < 0)
			unix_error("read\n");
		printf("%s\n", buf);
	}
	exit(0);
}
