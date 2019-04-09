#include "csapp.h"
void handler(int sig) {
	static int beeps = 0;
	
	printf("BEEP\n");
	if(++beeps < 3) 
		Alarm(1);
	else {
		printf("BOOM!");
		_exit(0);
	}
}

int main() {
	Signal(SIGALRM, handler);
	Alarm(1);

	while(1) { ;}
	printf("Main exit\n");
	exit(0);
}
