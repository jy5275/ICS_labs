#include "csapp.h"

int main() {
	pid_t pid[10000];
	int cnt = 0;
	for (cnt = 0; ; cnt++) {
		if ((pid[cnt] = fork()) == 0) {
			exit(0);
		}
		else if (pid[cnt] < 0)
			break;
	}
	printf("%d\n", cnt);
	for (int i = 0; i < cnt; i++)
		kill(pid[i], SIGKILL);
	while (waitpid(-1, NULL, 0));
	return 0;
}
