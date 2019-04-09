#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/wait.h>

//Print error message to standard error output stream
void unix_error(char *msg) {
	fprintf(stderr, "%s: %s\n", msg, strerror(errno));
	exit(0);
}

//Wrapped fork function, fork inspection included
pid_t Fork(void){
	pid_t pid;
	if ((pid = fork()) < 0)
		unix_error("Fork error");
	return pid;
}

int main(int argc, char *argv[]) {
	pid_t pid;
	int x = 1;
	printf("This line runs before fork. \n");

	pid = Fork();
	if (pid == 0) {       // Child Process
		x++;
		printf("child: x = %d\n", x);
		//pid_t myPID = getpid(), myParent = getppid();
		//printf("getpid() = %d, getppid() = %d\n", myPID, myParent);
	}
	else { 		// Parent Process
		x--;
		printf("parent: x = %d\n", x);
		//pid_t myPID = getpid(), myParent = getppid();
		//printf("getpid() = %d, getppid() = %d\n", myPID, myParent);
	}
	printf("\nThis line runs after fork. \n&x = %p. \nMy PID is %d, whle my father's PID is %d. \n\n", &x, getpid(), getppid());

	return 0;
}
