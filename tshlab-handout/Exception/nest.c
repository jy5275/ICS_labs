#include "csapp.h"

void sigfpe_handler(int sig) {
	long out_val;
	__asm__ volatile( 
		"movq %%rsp, %0 "  // ans = R[%rsp]
		: "=m" (out_val)
		: "m" (out_val)
	);
	printf("SIGFPE Caught! R[%%rsp] of SIGFPE handler is %lx\n", out_val);
	exit(0);
}
void sigint_handler(int sig) {
	long out_val;
	__asm__ volatile( 
		"movq %%rsp, %0 "  // ans = R[%rsp]
		: "=m" (out_val)
		: "m" (out_val)
	);
	printf("SIGINT Caught! R[%%rsp] of SIGINT handler is %lx\n", out_val);
	int a = 0;
	int b = 3 / a;
	exit(0);
}

int main() {
	long out_val;
	__asm__ volatile( 
		"movq %%rsp, %0 "  // ans = R[%rsp]
		: "=m" (out_val)
		: "m" (out_val)
	);
	printf("R[%%rsp] of main function is %lx\n", out_val);
	
	signal(SIGINT, sigint_handler);
	signal(SIGFPE, sigfpe_handler);
	Pause();
	return 0;
}


