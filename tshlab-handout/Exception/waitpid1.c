#include "csapp.h"
#define N 10

int main() {
	int status, i;
	pid_t pid[N], retpid;

	for (int i = 0; i < N; i++) 
		if ((pid[i] = Fork()) == 0) {
			/*if(i == 2) {  // Only executed on (pid==2)
				int b = 0;
				int a = 3 / b;
				printf("3 / 0 = %d\n", a);
			}*/
			Sleep(i);
			exit(100 + i);
		}

	/* Parent reaps N children in order */
	i = 0;
	while ((retpid = waitpid(-1, &status, 0)) > 0) {
		if(WIFEXITED(status))
			printf("child%d terminated normally with exit status = %d\n", retpid, WEXITSTATUS(status));
		else {
			//if (WIFSTOPPED) 
			//	printf("child%d stopsig = %d\n", pid, WSTOPSIG(status));
			printf("child%d termianted abnormally. \n", retpid);
		}
	}

	if(errno != ECHILD)
		unix_error("waitpid error");

	exit(0);
}
