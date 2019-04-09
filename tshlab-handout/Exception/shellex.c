#include "csapp.h"
#define MAXARGS 128
void eval(char *cmdline);
int parseline(char *buf, char **argv);
int builtin_command(char **argv);

/* Still runs in main process! */
int builtin_command(char **argv) { 
	if (!strcmp(argv[0], "quit"))  exit(0);   // Terminate shell's main process!
	if (!strcmp(argv[0], "&"))  return 1;
	return 0;     // Return 0 if argv contains '&' only
}

/* Still in main process! */
int parseline(char *buf, char *argv[]) {
	char *delim;
	int argc, bg;

	buf[strlen(buf)-1] = ' ';
	while (*buf && (*buf == ' '))  // Ignore leading spaces
		buf++;

	/* Build the argv list */
	argc = 0;
	while (delim = strchr(buf, ' ')) {  // Point to the next ' '
		argv[argc++] = buf; 	// Allocate argv[argc]'s pointer
		*delim = '\0';
		buf = delim + 1;	// Move buf forward
		while (*buf && (*buf == ' '))   // Ignore leading spaces
			buf++;
	}
	argv[argc] = NULL;
	if (argc == 0) 	return 1;   // Ignore blank line
	if ((bg = (*argv[argc-1] == '&')) != 0)
		argv[--argc] = NULL;    // We do not consider '&' an argument
	return bg;
}

void eval(char *cmdline) {   // Main process! Execute one time for each program
	char *argv[MAXARGS] = { NULL }, buf[MAXLINE];
	pid_t pid;
	strcpy(buf, cmdline);
	int bg = parseline(buf, argv); // Set argv[]
	if(argv[0] == NULL)  return;   // Ignore empty lines
	if(!builtin_command(argv)) {
		if((pid = Fork()) == 0) {  // Only runs in child process
			if(execve(argv[0], argv, environ) < 0) {
				printf("%s: Command not found. \n", argv[0]);
				exit(0);
			}
		}
		if(!bg) {    // Parent wait for fg process to terminate. Never runs in child process
			int status;
			if(waitpid(pid, &status, 0) < 0)
				unix_error("waitfg: waitpid error");
		}
		else  printf("%d %s", pid, cmdline);
	} return;
}

int main() {
	/* Main process */
	char cmdline[MAXLINE];
	while(1) {
		printf("getpgrp: %d\n", getpgrp());
		printf("> ");
		Fgets(cmdline, MAXLINE, stdin);
		if(feof(stdin))  exit(0);
		eval(cmdline);
	}
	return 0;
}
