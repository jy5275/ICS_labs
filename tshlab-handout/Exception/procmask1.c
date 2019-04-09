#include "csapp.h"

void handler(int sig) {
	int olderrno = errno;
	sigset_t mask_all, prev_all;
	pid_t pid;
	
	Sigfillset(&mask_all);
	while ((pid = waitpid(-1, NULL, 0)) > 0) {
		Sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
		deletejob(pid);
		Sigprocmask(SIG_SETMASK, &prev_all, NULL);
	}
	
	if (errno != ECHILD)
		Sio_error("waitpid error\n");
	errno = olderrno;
}

int main() {
	int pid;
	sigset_t mask_all, prev_all, mask_one;
	
	Sigfillset(&mask_all);
	Sigemptyset(&mask_one);
	Sigaddset(&mask_one, SIGCHLD);
	Signal(SIGCHLD, handler);
	initjobs();
	
	while(1) {
		Sigprocmask(SIG_BLOCK, &mask_one, &prev_one);
		if ((pid = Fork()) == 0) {
			Sigprocmask(SIG_SETMASK, &prev_one, NULL);  // Unblock SIGCHLD in child process
			Execve(argv[0], argv, NULL);
		}
		Sigprocmask(SIG_BLOCK, &mask_all, NULL);
		addjob(pid);
		SIgprocmask(SIG_SETMASK, &prev_one, NULL);
	}
	return 0;
}
