#include "csapp.h"
/* The default action of SIGINT is to terminate current process (exit(0)).
   But here we just want it to return normally.
   Because we still have something to print out in snooze.
*/
void sigtstp_handler(int sig) {
	printf("%d receive a SIGTSTP ... and wake up. \n", getpid());
	return;
}
void sigint_handler(int sig) { return; }
void sigchld_handler(int sig) {
	printf("Your child has expired. You should also die instantly. \n");
	return;
}
unsigned snooze(unsigned secs) {
	printf("%d before sleep\n", getpid());
	unsigned rc = sleep(secs);
	printf("%d slept for %d of %d seconds\n", getpid(), secs - rc, secs);

	_exit(0);
}

int main(int argc, char *argv[]) {
	unsigned u = atoi(argv[1]);
	pid_t pid;
	sigset_t mask, prev;
	if ((pid = fork()) == 0) {
		//signal(SIGTSTP, sigtstp_handler);
		signal(SIGINT, sigint_handler);
		snooze(u+2);
		_exit(0);
	}
	Sigemptyset(&mask);
	Sigaddset(&mask, SIGINT);
	Sigprocmask(SIG_BLOCK, &mask, &prev);
	//signal(SIGCHLD, sigchld_handler);
	snooze(u);
	
	waitpid(pid, NULL, 0);
	_exit(0);
}
