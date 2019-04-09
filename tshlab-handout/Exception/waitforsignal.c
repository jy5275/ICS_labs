#include "csapp.h"
volatile sig_atomic_t pid;

void sigchld_handler(int s) {
	int olderrno = errno;
	pid = waitpid(-1, NULL, 0);
	errno = olderrno;
}

void sigint_handler(int s) { }

int main() {
	sigset_t mask, prev;

	Signal(SIGCHLD, sigchld_handler);
	Signal(SIGINT, sigint_handler);
	Sigemptyset(&mask);
	Sigaddset(&mask, SIGCHLD);

	while(1) {
		char buf[10];
		read(STDIN_FILENO, buf, sizeof(buf));
		Sigprocmask(SIG_BLOCK, &mask, &prev);
		if (Fork() == 0 )
			exit(0);

		/* Parent and child process are racing to alter global variable pid */
		pid = 0; 
		Sigprocmask(SIG_SETMASK, &prev, NULL);

		/* Wait for SIGCHLD to be received */
		while(!pid) sleep(1); 

		printf(".");
	}
	exit(0);
}
