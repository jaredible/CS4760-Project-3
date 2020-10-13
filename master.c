/*
 * master.c 10/16/20
 * Jared Diehl (jmddnb@umsystem.edu)
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 * This program creates palin processes
 * to process the input file's strings.
 */

/* Standard library header files */
#include <ctype.h> // isdigit
#include <getopt.h> // getopt, optarg, optind
#include <signal.h> // signal
#include <stdarg.h> // va_end, va_list, va_start
#include <stdbool.h> // false, true
#include <stdio.h> // fprintf, perror, printf, sprintf, stderr, vsnprintf
#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS, atoi, exit
#include <string.h> // strcpy
#include <sys/ipc.h> // IPC_CREAT, IPC_RMID, ftok
#include <sys/shm.h> // shmat, shmctl, shmdt, shmget
#include <sys/stat.h> // S_IRUSR, S_IWUSR
#include <sys/time.h> // itimerval, setitimer
#include <sys/types.h> // pid_t
#include <sys/wait.h> // wait
#include <unistd.h> // F_OK, access, execl, fork

/* Programmer-defined header files */
#include "constant.h"
#include "helper.h"
#include "shared.h"

/* Definitions */
#define OPTIONS "hs:t:"

/* Private function prototypes */
static void usage(int);
static int load(char*);
static void spawn(int);
static void timer(int);
static void handler(int);
static void finalize(bool);

/* Private global variables */
static int n = TOTAL_CHILDREN_DEFAULT;
static int s = CONCURRENT_CHILDREN_DEFAULT;
static int t = TIMEOUT_DEFAULT;

/* Main program */
int main(int argc, char **argv) {
	init(argc, argv);
	
	bool ok = true;
	
	/* Get options */
	while (true) {
		int c = getopt(argc, argv, OPTIONS);
		
		if (c == -1) break;
		
		switch (c) {
			case 'h':
				/* Display usage information */
				usage(EXIT_SUCCESS);
			case 's':
				/* Ensure option s is valid and within range */
				if (!isdigit(*optarg) || (s = atoi(optarg)) < 0) {
					error("invalid concurrent children '%s'", optarg);
					ok = false;
				}
				break;
			case 't':
				/* Ensure option t is valid and within range */
				if (!isdigit(*optarg) || (t = atoi(optarg)) < 0) {
					error("invalid timeout '%s'", optarg);
					ok = false;
				}
				break;
			default:
				ok = false;
		}
	}
	
	/* Path of input file */
	char *path;
	
	/* Check input file */
	if ((path = argv[optind]) == NULL) {
		error("no argument supplied for infile");
		ok = false;
	} else if (access(path, F_OK) == -1) {
		error("infile does not exist '%s'", path);
		ok = false;
	}
	
	if (!ok) usage(EXIT_FAILURE);
	
	/* Register Ctrl+C signal handler */
	sigact(SIGINT, &handler);
	
	/* Initialize output files */
	rtouch("output.log");
	rtouch("palin.out");
	rtouch("nopalin.out");
	
	/* Initialize shared memory and semaphores */
	shmAllocate(true);
	semAllocate(true, 3);
	
	/* Output the options */
	char msg[BUFFER_LENGTH];
	sprintf(msg, "%s: Starting with options: s=%d, t=%d\n", ftime(), s, t);
	fprintf(stderr, msg);
	flog("output.log", msg);
	
	/* Initialize strings */
	int c = load(path);
	
	/* Output the string count */
	sprintf(msg, "%s: Processing %d strings\n", ftime(), c);
	fprintf(stderr, msg);
	flog("output.log", msg);
	
	/* Initialize timer */
	if (t == 0) {
		finalize(true);
		exit(EXIT_SUCCESS);
	} else timer(t);
	
	/* Clamp the options */
	n = MIN(c, CHILD_COUNT);
	s = MIN(s, n);
	
	/* Temporary variables */
	int i, j, status;
	
	/* Spawn initial children */
	for (i = 0; i < s; i++)
		spawn(i);
	
	/* Keep trying to spawn children after one dies */
	for (j = n; j > 0 && s > 0; j--) {
		/* Wait for a child to die */
		wait(&status);
		/* Output that a child process is finished */
		if (WIFEXITED(status)) flog("output.log", "%s: Process %d finished\n", ftime(), WEXITSTATUS(status));
		/* Attempt to spawn a child */
		if (i < n) spawn(i++);
	}
	
	finalize(true);
	
	return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}

/*
 * usage(status)
 * -------------
 * Displays a help message or usage information depending on status, and exits.
 */
static void usage(int status) {
	if (status != EXIT_SUCCESS) {
		/* Display help message */
		fprintf(stderr, "Try '%s -h' for more information\n", getProgramName());
	} else {
		/* Display usage information */
		printf("NAME\n");
		printf("       %s - palindrome finder\n", getProgramName());
		printf("USAGE\n");
		printf("       %s -h\n", getProgramName());
		printf("       %s [-s x] [-t time] infile\n", getProgramName());
		printf("DESCRIPTION\n");
		printf("       -h       : Print a help message or usage, and exit\n");
		printf("       -s x     : Number of children allowed to exist concurrently (default 2)\n");
		printf("       -t time  : Time, in seconds, after which the program will terminate (default 100)\n");
	}
	exit(status);
}

/*
 * load(path)
 * ----------
 * From the file specified by path, loads into shared memory all its strings.
 */
static int load(char *path) {
	/* Attempt to open the file */
	FILE *fp;
	if ((fp = fopen(path, "r")) == NULL) crash("fopen");
	
	int i = 0;
	char *line;
	size_t len = 0;
	ssize_t read;
	
	/* Copy every line with a line index less than n into shared memory */
	for (; i < n && (read = getline(&line, &len, fp)) != -1; i++) {
		crnl(line); /* Remove newline character */
		setString(i, line); /* Copy line into shared memory at index i*/
	}
	
	if (fclose(fp) == EOF) crash("fclose");
	if (line) free(line);
	
	return i;
}

/*
 * spawn(index)
 * ------------
 * Creates a new child to execute the palin program.
 */
static void spawn(int index) {
	/* Attempt to create a new child process */
	pid_t cpid = fork();
	if (cpid == -1) crash("fork");
	else if (cpid == 0) {
		/* Set this new process' PGID */
		if (index == 0) setCpgid(getpid());
		setpgid(0, getCpgid());
		
		/* Output that a child process is starting */
		flog("output.log", "%s: Process %d starting\n", ftime(), index);
		
		/* Execute palin program */
		char cindex[BUFFER_LENGTH];
		sprintf(cindex, "%d", index);
		execl("./palin", "palin", cindex, (char*) NULL);
		
		exit(EXIT_SUCCESS);
	}
}

/* timer(seconds)
 * --------------
 * Sets up the timer signal handler.
 */
static void timer(int seconds) {
	/* Register timer signal handler */
	sigact(SIGALRM, &handler);
	
	/* Initialize real timer */
	struct itimerval itv;
	itv.it_value.tv_sec = seconds;
	itv.it_value.tv_usec = 0;
	itv.it_interval.tv_sec = 0;
	itv.it_interval.tv_usec = 0;
	if (setitimer(ITIMER_REAL, &itv, NULL) == -1) crash("setitimer");
}

/*
 * handler(signal)
 * ---------------
 * Handles all programmer-defined signals.
 */
static void handler(int signal) {
	/* Output the caught signal */
	char msg[BUFFER_LENGTH];
	sprintf(msg, "%s: Signal %s caught\n", ftime(), signal == SIGALRM ? "timeout" : "interrupt");
	fprintf(stderr, msg);
	flog("output.log", msg);
	
	/* Kill all children */
	killpg(getCpgid(), signal == SIGALRM ? SIGUSR1 : SIGTERM);
	while (wait(NULL) > 0);
	
	finalize(false);
	exit(EXIT_SUCCESS);
}

/*
 * finalize(finished)
 * ------------------
 * Outputs an exit message and releases resources.
 */
static void finalize(bool finished) {
	/* Output exit status */
	char msg[BUFFER_LENGTH];
	sprintf(msg, "%s: Exiting %sfinished\n", ftime(), finished ? "" : "un");
	fprintf(stderr, msg);
	flog("output.log", msg);
	
	/* Release shared memory and semaphores */
	shmRelease();
	semRelease();
}
