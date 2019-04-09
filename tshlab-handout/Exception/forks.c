#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#define N 3

void fork12() {
	pid_t pid[N];
	int child_status;

	for (int i = 0; i < N; i++) 
		if ((pid[i] = fork()) == 0) {
			while(1) ;
		}

	for (int i = 0; i < N; i++) {
		printf("Killing process %d\n", pid[i]);
		kill(pid[i], SIGINT);
	}

	for (int i = 0; i < N; i++) {
		pid_t wpid = wait(&child_status);
		if (WIFEXITED(child_status))
			printf("Child %d terminated with exit status %d\n", wpid, WEXITSTATUS(child_status));
		else printf("Child %d terminated abnormally. \n", wpid);
	}
}

int main() {
	fork12();
	return 0;
}
