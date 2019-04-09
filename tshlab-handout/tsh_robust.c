/* tsh - A tiny shell program with job control
 * 蒋衍 1600013239 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>

/* Misc manifest constants */
#define MAXLINE    1024   /* max line size */
#define MAXARGS     128   /* max args on a command line */
#define MAXJOBS      16   /* max jobs at any point in time */
#define MAXJID    1<<16   /* max job ID */

/* Job states */
#define UNDEF         0   /* undefined */
#define FG            1   /* running in foreground */
#define BG            2   /* running in background */
#define ST            3   /* stopped */

/* 
 * Jobs states: FG (foreground), BG (background), ST (stopped)
 * Job state transitions and enabling actions:
 *     FG -> ST  : ctrl-z
 *     ST -> FG  : fg command
 *     ST -> BG  : bg command
 *     BG -> FG  : fg command
 * At most 1 job can be in the FG state. */

/* Parsing states */
#define ST_NORMAL   0x0   /* next token is an argument */
#define ST_INFILE   0x1   /* next token is the input file */
#define ST_OUTFILE  0x2   /* next token is the output file */

/* Global variables */
extern char **environ;      /* defined in libc */
char prompt[] = "tsh> ";
int verbose = 0;            /* if true, print additional output */
int nextjid = 1;            /* next job ID to allocate */
char sbuf[MAXLINE];         /* for composing sprintf messages */
volatile sig_atomic_t waitfg;

struct job_t {
    pid_t pid;
    int jid;                /* job ID [1, 2, ...] */
    int state;              /* UNDEF, BG, FG, or ST */
    char cmdline[MAXLINE];
};
struct job_t job_list[MAXJOBS];

struct cmdline_tokens {
    int argc;               /* Number of arguments */
    char *argv[MAXARGS];    /* The arguments list */
    char *infile, *outfile;
    enum builtins_t {       /* Indicates if argv[0] is a builtin command */
        BUILTIN_NONE, BUILTIN_QUIT, 
        BUILTIN_JOBS, BUILTIN_BG, 
        BUILTIN_FG} builtins;
};
/* End global variables */

/* Function prototypes */
void eval(char *cmdline);
void sigchld_handler(int sig);
void sigtstp_handler(int sig);
void sigint_handler(int sig);

/* Here are helper routines that we've provided for you */
int parseline(const char *cmdline, struct cmdline_tokens *tok);  // Needn't change these functions...
void sigquit_handler(int sig);
void turnbg(struct job_t *pbg_job);
void clearjob(struct job_t *job);
void initjobs(struct job_t *job_list);
int maxjid(struct job_t *job_list); 
int addjob(struct job_t *job_list, pid_t pid, int state, char *cmdline);
int deletejob(struct job_t *job_list, pid_t pid); 
pid_t fgpid(struct job_t *job_list);
struct job_t *getjobpid(struct job_t *job_list, pid_t pid);
struct job_t *getjobjid(struct job_t *job_list, int jid); 
int pid2jid(pid_t pid); 
void listjobs(struct job_t *job_list, int output_fd);

void usage(void);
void unix_error(char *msg);
void app_error(char *msg);
ssize_t sio_puts(char s[]);
ssize_t sio_putl(long v);
void sio_error(char s[]);

typedef void handler_t(int);
handler_t *Signal(int signum, handler_t *handler);

/* main - The shell's main routine */
int main (int argc, char **argv) {
	char c, cmdline[MAXLINE];      /* cmdline for fgets */
	int emit_prompt = 1;
	
	/* Redirect stderr to stdout (so that driver will get all output on the pipe connected to stdout) */
    dup2(1, 2);
    while ((c = getopt(argc, argv, "hvp")) != EOF) {   /* Parse the command line */
        switch (c) {
        case 'h': usage(); break;        	/* help */
        case 'v': verbose = 1; break;    	/* emit additional diagnostic info */
        case 'p': emit_prompt = 0; break;   /* don't print a prompt, handy for automatic testing */
        default: usage();
        }
    }
    /* These are the ones you will need to implement */
    Signal(SIGINT,  sigint_handler);   /* ctrl-c */
    Signal(SIGTSTP, sigtstp_handler);  /* ctrl-z */
    Signal(SIGCHLD, sigchld_handler);  /* Terminated or stopped child */
    Signal(SIGTTIN, SIG_IGN);
    Signal(SIGTTOU, SIG_IGN);
    Signal(SIGQUIT, sigquit_handler);  // A clean way to kill the shell
    initjobs(job_list);

    /* Execute the shell's read/eval loop */
    while (1) {
        if (emit_prompt) {
            printf("%s", prompt);
            fflush(stdout);
        }
        if ((fgets(cmdline, MAXLINE, stdin) == NULL) && ferror(stdin))
            app_error("fgets error");
        if (feof(stdin)) {   /* End of file (ctrl-d) */
            printf ("\n");
            fflush(stdout);
            fflush(stderr);
            exit(0);
        }
        /* Remove the trailing newline */
        cmdline[strlen(cmdline)-1] = '\0';
        /* Evaluate the command line */
        eval(cmdline);
        fflush(stdout);
        fflush(stdout);
    } 
    exit(0); /* control never reaches here */
}

/* eval - Evaluate the command line that the user has just typed in
 * 
 * If the user has requested a built-in command (quit, jobs, bg or fg)
 * execute it immediately. Otherwise, fork a child process and
 * run the job in the context of the child. 
 * If the job is running fg, wait for it to terminate and then return.  
 * Note:
 * each child process must have a unique process group ID so that our
 * bg children don't receive SIGINT (SIGTSTP) from the kernel
 * when we type ctrl-c (ctrl-z) at the keyboard.  
 */
void eval(char *cmdline) {
    int bg;              /* should the job run in bg or fg? */
    char buf[4 * MAXLINE];
	struct job_t *pbg_job = NULL;
	struct job_t *pfg_job = NULL;
    struct cmdline_tokens tok;
    sigset_t mask_3, prev, primal, mask_all;
	pid_t pid;
	
	/* Parse command line */
	bg = parseline(cmdline, &tok); 
	if (bg == -1) return;    /* parsing error */
	if (tok.argv[0] == NULL) return;   /* ignore empty lines */
	
	sigfillset(&mask_all);
	sigemptyset(&mask_3); 
	sigemptyset(&primal);
	sigaddset(&mask_3, SIGCHLD);
	sigaddset(&mask_3, SIGINT);
	sigaddset(&mask_3, SIGTSTP);
	
	/* Open files */
	if (tok.infile) {
		int fd_in = open(tok.infile, O_RDONLY);
		dup2(fd_in, 0);
	}
	if (tok.outfile) {
		int fd_out = open(tok.outfile, O_WRONLY);
		dup2(fd_out, 1); dup2(fd_out, 2);
	}
	
	if (tok.builtins == BUILTIN_NONE) {      // fork and run!
		sigprocmask(SIG_BLOCK, &mask_3, &prev);
		if ((pid = fork()) == 0) {
		/* =========================== Only runs in child process ================================== */
			sigprocmask(SIG_SETMASK, &prev, NULL);
			setpgid(0, 0);                   // New process group apart from tsh's routine!
			
			Signal(SIGINT,  SIG_DFL);
			Signal(SIGTSTP, SIG_DFL);
			Signal(SIGCHLD, SIG_DFL);
			Signal(SIGTTIN, SIG_DFL);
			Signal(SIGTTOU, SIG_DFL);
			Signal(SIGQUIT, SIG_DFL);
			
			if (execve(tok.argv[0], tok.argv, environ) < 0) {     // Print error if file argv[0] doesn't exist
				printf("%s: Command not found. \n", tok.argv[0]);
				exit(0);
			}
		}
		/* =========================== End of child-exclusive code ================================== */
		
		if (!bg) {      // Waiting for fg job to terminate
			addjob(job_list, pid, FG, cmdline);
			waitfg = 1;
			while (waitfg) {
				sigsuspend(&prev); 
			}
		}
		else {          // Background job
			addjob(job_list, pid, BG, cmdline);
			pbg_job = getjobpid(job_list, pid);
			sprintf(buf, "[%d] (%d) %s\n", pbg_job->jid, pid, cmdline);
			write(1, buf, strlen(buf));
		}
		sigprocmask(SIG_SETMASK, &prev, NULL);
		return;
	} 
	else if (tok.builtins == BUILTIN_JOBS) 
		listjobs(job_list, 1);
	else if (tok.builtins == BUILTIN_BG) {
		int bg_pid, bg_jid;
		if (tok.argv[1][0] == '%') {
			bg_jid = atoi(&tok.argv[1][1]);    // bg_jid -> pbg_job
			pbg_job = getjobjid(job_list, bg_jid);			
			if (pbg_job == NULL) {             // Such jid does not exist
				sprintf(buf, "%%%d: No such job\n", bg_jid);
				write(1, buf, strlen(buf));
				return;
			}
		}
		else if (tok.argv[1][0] >= '0' && tok.argv[1][0] <= '9') {
			bg_pid = atoi(tok.argv[1]);
			pbg_job = getjobpid(job_list, bg_pid);
			if (pbg_job == NULL) {
				sprintf(buf, "(%d): No such process\n", bg_pid);
				write(1, buf, strlen(buf));
				return;
			}
		}
		turnbg(pbg_job);
		return;
	}
	else if (tok.builtins == BUILTIN_FG) {
		int fg_pid, fg_jid;
		
		/* Block SIGCHLD, or we'll suspend forever if FG job terminates early */
		sigset_t mask_chld, prev;
		sigemptyset(&mask_chld);
		sigaddset(&mask_chld, SIGCHLD);
		sigprocmask(SIG_BLOCK, &mask_chld, &prev);
		
		if (tok.argv[1][0] == '%') {
			fg_jid = atoi(&tok.argv[1][1]);    // fg_jid -> pfg_job
			pfg_job = getjobjid(job_list, fg_jid);			
			if (pfg_job == NULL) {             // Such jid does not exist
				sprintf(buf, "%%%d: No such job\n", fg_jid);
				write(1, buf, strlen(buf));
				return;
			}
		}
		else if (tok.argv[1][0] >= '0' && tok.argv[1][0] <= '9') {
			fg_pid = atoi(tok.argv[1]);
			pfg_job = getjobpid(job_list, fg_pid);
			if (pfg_job == NULL) {
				sprintf(buf, "(%d): No such process\n", fg_pid);
				write(1, buf, strlen(buf));
				return;
			}
		}
		pfg_job->state = FG;
		kill(-pfg_job->pid, SIGCONT);
		/* Waiting for the FG job to terminate */
		while (pfg_job->jid != 0) {     
			/* Stay in loop while the FG job still exists. 
			   If interrupted by a SIGCHLD from another process, 
			   that process could also be reaped as usual.
			   Only the kernal has reaped all zombies can we return from sigchld_handler,
			    (then sigsuspend) to check wheather the loop condition still satisfy. 
			*/
			sigsuspend(&prev);
		}
		sigprocmask(SIG_SETMASK, &prev, NULL);
		return;
	}
	else if (tok.builtins == BUILTIN_QUIT) 
		exit(0);   // The only exit of tsh's routine (except for using kill to send SIGKILL to routine directly)
	
	return;
}

void turnbg(struct job_t *pbg_job) {
	char buf[4 * MAXLINE];
	kill(-pbg_job->pid, SIGCONT);
	pbg_job->state = BG;
	sprintf(buf, "[%d] (%d) %s\n", pbg_job->jid, pbg_job->pid, pbg_job->cmdline);
	write(1, buf, strlen(buf));
}

/* parseline - Parse the command line and build the argv array.
 * 
 * Parameters:
 *   cmdline:  The command line, in the form:
 *
 *                command [arguments...] [< infile] [> oufile] [&]
 *
 *   tok:      Pointer to a cmdline_tokens structure. The elements of this
 *             structure will be populated with the parsed tokens. Characters 
 *             enclosed in single or double quotes are treated as a single
 *             argument. 
 * Returns:
 *   1:        if the user has requested a BG job
 *   0:        if the user has requested a FG job  
 *  -1:        if cmdline is incorrectly formatted
 * 
 * Note:       The string elements of tok (e.g., argv[], infile, outfile) 
 *             are statically allocated inside parseline() and will be 
 *             overwritten the next time this function is invoked.
 */
int parseline(const char *cmdline, struct cmdline_tokens *ptok) {

    static char array[MAXLINE];          /* holds local copy of command line */
    const char delims[10] = " \t\r\n";   /* argument delimiters (white-space) */
    char *buf = array;                   /* ptr that traverses command line */
    char *next;                          /* ptr to the end of the current arg */
    char *endbuf;                        /* ptr to end of cmdline string */
    int is_bg;                           /* bg job? */
    int parsing_state;                   /* is the next token I/O file ? */

    if (cmdline == NULL) {
        (void) fprintf(stderr, "Error: command line is NULL\n");
        return -1;
    }

    (void) strncpy(buf, cmdline, MAXLINE);
    endbuf = buf + strlen(buf);
    ptok->infile = NULL; ptok->outfile = NULL;

    /* Build the argv list */
    parsing_state = ST_NORMAL;
    ptok->argc = 0;

    while (buf < endbuf) {
        /* Skip the white-spaces */
        buf += strspn (buf, delims);
        if (buf >= endbuf) break;

        if (*buf == '<') {       /* I/O redirection specifiers */
            if (ptok->infile) {
                (void) fprintf(stderr, "Error: Ambiguous I/O redirection\n");
                return -1;
            }
            parsing_state |= ST_INFILE;
            buf++;
            continue;
        }
        else if (*buf == '>') {
            if (ptok->outfile) {
                (void) fprintf(stderr, "Error: Ambiguous I/O redirection\n");
                return -1;
            }
            parsing_state |= ST_OUTFILE;
            buf++;
            continue;
        }
        else if (*buf == '\'' || *buf == '\"') {   /* quoted tokens */
            buf++;
            next = strchr (buf, *(buf-1));
        } 
        else   /* Most cases, find the next delimiter */
			next = buf + strcspn (buf, delims);   // Point to the next ' '
        
        if (next == NULL) { /* Returned by strchr(); means closing quote not found. */
            (void) fprintf (stderr, "Error: unmatched %c.\n", *(buf-1));
            return -1;
        }
        *next = '\0';   /* Terminate the token */

        /* Record the token as either the next argument or the i/o file */
        switch (parsing_state) {
        case ST_NORMAL: ptok->argv[ptok->argc++] = buf; break;
        case ST_INFILE: ptok->infile = buf; break;
        case ST_OUTFILE: ptok->outfile = buf; break;
        default:
            (void) fprintf(stderr, "Error: Ambiguous I/O redirection\n");
            return -1;
        }
        parsing_state = ST_NORMAL;
	
        /* Check if argv is full */
        if (ptok->argc >= MAXARGS-1) break;
        buf = next + 1;
    }

    if (parsing_state != ST_NORMAL) {
        (void) fprintf(stderr, "Error: must provide file name for redirection\n");
        return -1;
    }

    /* The argument list must end with a NULL pointer */
    ptok->argv[ptok->argc] = NULL;

    if (ptok->argc == 0)  /* ignore blank line */  return 1;

    if (!strcmp(ptok->argv[0], "quit"))                 /* quit command */
        ptok->builtins = BUILTIN_QUIT;
     else if (!strcmp(ptok->argv[0], "jobs"))           /* jobs command */
        ptok->builtins = BUILTIN_JOBS;
     else if (!strcmp(ptok->argv[0], "bg"))             /* bg command */
        ptok->builtins = BUILTIN_BG;
     else if (!strcmp(ptok->argv[0], "fg"))             /* fg command */
        ptok->builtins = BUILTIN_FG;
     else 
        ptok->builtins = BUILTIN_NONE;

    /* Should the job run bg? */
    if ((is_bg = (*ptok->argv[ptok->argc-1] == '&')) != 0)
        ptok->argv[--ptok->argc] = NULL;

    return is_bg;
}


/*****************
 * Signal handlers
 *****************/ 
/* sigchld_handler - The kernel sends a SIGCHLD to the shell whenever
 *     a child job terminates (becomes a zombie), or stops because it
 *     received a SIGSTOP, SIGTSTP, SIGTTIN or SIGTTOU signal. The 
 *     handler reaps all available zombie children, but doesn't wait 
 *     for any other currently running children to terminate.  
 */
void sigchld_handler(int sig) {
	int olderrno = errno;
	sigset_t mask_all, prev;
	pid_t pid;
	int status;	waitfg = 0;
	sigfillset(&mask_all);
	
	while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0) {
		/* Block all signals while visiting global */
		sigprocmask(SIG_BLOCK, &mask_all, &prev); 
		struct job_t *pjob = getjobpid(job_list, pid);
		sigprocmask(SIG_SETMASK, &prev, NULL);
		
		if (WIFSTOPPED(status)) {
		/* Suspended(stopped) by another signal */
			pjob->state = ST;
			sio_puts("Job ["); sio_putl(pjob->jid); sio_puts("] ("); 
			sio_putl(pid); sio_puts(") stopped by signal "); 
			sio_putl(WSTOPSIG(status)); sio_puts("\n");
		}
		else if (WIFSIGNALED(status)) {
		/* Terminated by an uncaught signal */
			sio_puts("Job ["); sio_putl(pjob->jid);
			sio_puts("] ("); sio_putl(pid); 
			sio_puts(") terminated by signal "); sio_putl(WTERMSIG(status)); sio_puts("\n");
			
			sigprocmask(SIG_BLOCK, &mask_all, &prev); 
			deletejob(job_list, pid);
			sigprocmask(SIG_SETMASK, &prev, NULL);
			
		}
		else if (WIFEXITED(status)) {
		/* Normal termination by exit */
			sigprocmask(SIG_BLOCK, &mask_all, &prev); 
			deletejob(job_list, pid);
			sigprocmask(SIG_SETMASK, &prev, NULL);
		}
	}
	errno = olderrno;
}
 
/* sigint_handler - The kernel sends a SIGINT to the shell whenver the
 * user types ctrl-c at the keyboard.  Catch it and send it along
 * to the foreground job.  
 */
void sigint_handler(int sig) {	
	int olderrno = errno;
	sigset_t mask_all, prev;
	sigfillset(&mask_all);
	
	sigprocmask(SIG_BLOCK, &mask_all, &prev); 
	pid_t fg_pid = fgpid(job_list);
	sigprocmask(SIG_SETMASK, &prev, NULL);

	if (fg_pid) 
		kill(-fg_pid, SIGINT);   // Send SIGINT to each process in the fg group
	errno = olderrno;
    return;
}

/* sigtstp_handler - The kernel sends a SIGTSTP to the shell whenever
 *     the user types ctrl-z at the keyboard. Catch it and suspend the
 *     foreground job by sending it a SIGTSTP.  
 */
void sigtstp_handler(int sig) {
	int olderrno = errno;
	pid_t fg_pid = fgpid(job_list);
	//waitfg = 0;
	if (fg_pid)
		kill(-fg_pid, SIGTSTP);   // Send SIGTSTP to each process in the fg group
	errno = olderrno;
    return;
}

/* sigquit_handler - The driver program can gracefully terminate the
 *    child shell by sending it a SIGQUIT signal.
 */
void sigquit_handler(int sig) {
    sio_error("Terminating after receipt of SIGQUIT signal\n");
    _exit(0);
}

/*********************
 * End signal handlers
 *********************/

/***********************************************
 * Helper routines that manipulate the job list
 **********************************************/

/* clearjob - Clear the entries in a job struct */
void clearjob(struct job_t *job) {
    job->pid = 0;
    job->jid = 0;
    job->state = UNDEF;
    job->cmdline[0] = '\0';
}

/* initjobs - Initialize the job list */
void initjobs(struct job_t *job_list) {
    for (int i = 0; i < MAXJOBS; i++)
        clearjob(&job_list[i]);
}

/* maxjid - Returns largest allocated job ID */
int maxjid(struct job_t *job_list) {
    int max = 0;
    for (int i = 0; i < MAXJOBS; i++)
        if (job_list[i].jid > max)
            max = job_list[i].jid;
    return max;
}

int addjob(struct job_t *job_list, pid_t pid, int state, char *cmdline) {
    if (pid < 1) return 0;

    for (int i = 0; i < MAXJOBS; i++) {
        if (job_list[i].pid == 0) {
            job_list[i].pid = pid;
            job_list[i].state = state;
            job_list[i].jid = nextjid++;
            
            if (nextjid > MAXJOBS) nextjid = 1;
            strcpy(job_list[i].cmdline, cmdline);
            
            if(verbose) 
            	printf("Added job [%d] %d %s\n", job_list[i].jid, job_list[i].pid, job_list[i].cmdline);
            return 1;
        }
    }
    printf("Tried to create too many jobs\n");
    return 0;
}

/* deletejob - Delete a job whose PID=pid from the job list */
int deletejob(struct job_t *job_list, pid_t pid) {
    if (pid < 1) return 0;
    for (int i = 0; i < MAXJOBS; i++) {
        if (job_list[i].pid == pid) {
            clearjob(&job_list[i]);
            nextjid = maxjid(job_list) + 1;
            return 1;
        }
    }
    return 0;
}

/* fgpid - Return PID of current foreground job, 0 if no such job */
pid_t fgpid(struct job_t *job_list) {
    for (int i = 0; i < MAXJOBS; i++)
        if (job_list[i].state == FG)
            return job_list[i].pid;
    return 0;
}

/* getjobpid  - Find a job (by PID) on the job list */
struct job_t *getjobpid(struct job_t *job_list, pid_t pid) {
    if (pid < 1) return NULL;
    for (int i = 0; i < MAXJOBS; i++)
        if (job_list[i].pid == pid)
            return &job_list[i];
    return NULL;
}

/* getjobjid  - Find a job (by JID) on the job list */
struct job_t *getjobjid(struct job_t *job_list, int jid) {
    if (jid < 1)
        return NULL;
    for (int i = 0; i < MAXJOBS; i++)
        if (job_list[i].jid == jid)
            return &job_list[i];
    return NULL;
}

/* pid2jid - Map process ID to job ID */
int pid2jid(pid_t pid) {
    if (pid < 1) return 0;
    for (int i = 0; i < MAXJOBS; i++)
        if (job_list[i].pid == pid) {
            return job_list[i].jid;
        }
    return 0;
}

void listjobs(struct job_t *job_list, int output_fd) {
    char buf[4 * MAXLINE];
    for (int i = 0; i < MAXJOBS; i++) {
        memset(buf, '\0', MAXLINE);
        if (job_list[i].pid != 0) {
            sprintf(buf, "[%d] (%d) ", job_list[i].jid, job_list[i].pid);
            if(write(output_fd, buf, strlen(buf)) < 0) {
                fprintf(stderr, "Error writing to output file\n");
                exit(1);
            }
            memset(buf, '\0', MAXLINE);
            switch (job_list[i].state) {
            case BG:
                sprintf(buf, "Running    ");
                break;
            case FG:
                sprintf(buf, "Foreground ");
                break;
            case ST:
                sprintf(buf, "Stopped    ");
                break;
            default:
                sprintf(buf, "listjobs: Internal error: job[%d].state=%d ", i, job_list[i].state);
            }
            if(write(output_fd, buf, strlen(buf)) < 0) {
                fprintf(stderr, "Error writing to output file\n");
                exit(1);
            }
            memset(buf, '\0', MAXLINE);
            sprintf(buf, "%s\n", job_list[i].cmdline);
            if(write(output_fd, buf, strlen(buf)) < 0) {
                fprintf(stderr, "Error writing to output file\n");
                exit(1);
            }
        }
    }
}
/******************************
 * end job list helper routines
 ******************************/

/***********************
 * Other helper routines
 ***********************/

/*
 * usage - print a help message
 */
void usage(void) {
    printf("Usage: shell [-hvp]\n");
    printf("   -h   print this message\n");
    printf("   -v   print additional diagnostic information\n");
    printf("   -p   do not emit a command prompt\n");
    exit(1);
}

/*
 * unix_error - unix-style error routine
 */
void unix_error(char *msg){
    fprintf(stdout, "%s: %s\n", msg, strerror(errno));
    exit(1);
}

/* app_error - application-style error routine */
void app_error(char *msg) {
    fprintf(stdout, "%s\n", msg);
    exit(1);
}

/* Private sio_functions */
/* sio_reverse - Reverse a string (from K&R) */
static void sio_reverse(char s[]) {
    int c, i, j;

    for (i = 0, j = strlen(s)-1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

/* sio_ltoa - Convert long to base b string (from K&R) */
static void sio_ltoa(long v, char s[], int b) {
    int c, i = 0;
    
    do {  
        s[i++] = ((c = (v % b)) < 10)  ?  c + '0' : c - 10 + 'a';
    } while ((v /= b) > 0);
    s[i] = '\0';
    sio_reverse(s);
}

/* sio_strlen - Return length of string (from K&R) */
static size_t sio_strlen(char s[]) {
    int i = 0;

    while (s[i] != '\0')
        ++i;
    return i;
}

/* Public Sio functions */
ssize_t sio_puts(char s[]) { /* Put string */
    return write(STDOUT_FILENO, s, sio_strlen(s));
}

ssize_t sio_putl(long v) { /* Put long */
    char s[128];
    sio_ltoa(v, s, 10); /* Based on K&R itoa() */ 
    return sio_puts(s);
}

void sio_error(char s[]) { /* Put error message and exit */
    sio_puts(s);
    _exit(1);
}

/* Signal - wrapper for the sigaction function */
handler_t *Signal(int signum, handler_t *handler) {
    struct sigaction action, old_action;

    action.sa_handler = handler;  
    sigemptyset(&action.sa_mask); /* block sigs of type being handled */
    action.sa_flags = SA_RESTART; /* restart syscalls if possible */

    if (sigaction(signum, &action, &old_action) < 0)
        unix_error("Signal error");
    return (old_action.sa_handler);
}

