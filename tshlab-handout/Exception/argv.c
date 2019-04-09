#include <stdio.h>
int main (int argc, char *argv[]) {
	char s[100];
	scanf("%s", s);
	printf("Input: %s\n", s);

	for (int i = 0; i < argc; i++) 
		printf("argv[%d] = \"%s\"\n", i, argv[i]);
	return 0;
}
