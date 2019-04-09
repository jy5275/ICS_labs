#include <stdio.h>
int main(int argc, char *argv[], char *envp[]) {
	printf("Command-ine arguments: \n");
	for (int i = 0; i < argc; i++) 
		printf("argv[%2d]: %s\n", i, argv[i]);
	printf("Environment variables: \n");
	for (int i = 0; ; i++) {
		if(!envp[i]) break;
		printf("envp[%2d]: %s\n", i, envp[i]);
	}
	return 0;
}
