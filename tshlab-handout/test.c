#include <stdio.h>
#include "csapp.h"
void sigint_handler(int sig) {
	sio_puts("In handler - before another SIGINT\r\n");
	sleep(5);
	
	sio_puts("In handler - after another SIGINT.\r\n");
	exit(0);
}
int main(int argc, char *argv[]) {
	Signal(SIGINT, sigint_handler);
	int a = 0;
	sleep(atoi(argv[1]));
	//printf("Nonexisting num: %d\r\n", 3 / a);
	
	return 0;
}
