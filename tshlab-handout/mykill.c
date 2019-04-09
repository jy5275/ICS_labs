#include "csapp.h"
#define N 10
void sigchld_handler(int sig) {
	sio_puts("handler!\r\n");
	return;
}
void fork_kill() {
	pid_t pid[N];
	int child_status;
	
	sigset_t mask, prev;
	Sigemptyset(&mask);
	Sigaddset(&mask, SIGINT);
	Signal(SIGINT, SIG_IGN);
	/* Block SIGINT */
	//Sigprocmask(SIG_BLOCK, &mask, &prev);
	for (int i = 0; i < N; i++) {
		if((pid[i] = fork()) == 0) {
			printf("%d, getpgrp: %d\n", getpid(), getpgrp());
			//signal(SIGINT, SIG_IGN);
			Pause();
		}
	}
	/* Unblock SIGINT */
	//Sigprocmask(SIG_SETMASK, &prev, NULL); 
	
	Signal(SIGINT, SIG_DFL);
	for (int i = 0; i < N; i++) {
		//printf("Send SIGSTOP to process %d. \n", pid[i]);
		//kill(pid[i], SIGSTOP);
	}
	sleep(1);
	for (int i = 0; i < N; i++) {
		pid_t wpid = waitpid(pid[i], &child_status, WUNTRACED);  // will be permanently waiting if send SIGINT
		if (WIFSIGNALED(child_status)) 
			printf("Child %d terminated with signal %d\n", wpid, WTERMSIG(child_status));
		else if (WIFSTOPPED(child_status)) {
			printf("Child %d stopped by signal %d.\n", wpid, WSTOPSIG(child_status));
		}
		else printf("Child %d terminated abnormally\n", wpid);
	}
}
int main() {
	pid_t pid;
	fork_kill();
	Pause();
	exit(0);
}
