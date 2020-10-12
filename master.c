/*
 * master.c 10/16/20
 * Jared Diehl (jmddnb@umsystem.edu)
 */

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
#include <unistd.h> // execl, fork

#include "constant.h"
#include "helper.h"
#include "shared.h"

static void usage(int);
static int load(char*);
static void spawn(int);
static void timer(int);
static void handler(int);
static void finalize(bool);

static int n = TOTAL_CHILDREN_DEFAULT;
static int s = CONCURRENT_CHILDREN_DEFAULT;
static int t = TIMEOUT_DEFAULT;

int main(int argc, char** argv) {
	init(argc, argv);
	
	sigact(SIGINT, &handler);
	
	rtouch("palin.out");
	rtouch("nopalin.out");
	rtouch("output.log");
	
	bool ok = true;
	
	while (true) {
		int c = getopt(argc, argv, "hs:t:");
		
		if (c == -1) break;
		
		switch (c) {
			case 'h':
				usage(EXIT_SUCCESS);
			case 's':
				if (!isdigit(*optarg) || (s = atoi(optarg)) < 0) {
					error("invalid concurrent children '%s'", optarg);
					ok = false;
				}
				break;
			case 't':
				if (!isdigit(*optarg) || (t = atoi(optarg)) < 0) {
					error("invalid timeout '%s'", optarg);
					ok = false;
				}
				break;
			default:
				ok = false;
		}
	}
	
	if (!ok) usage(EXIT_FAILURE);
	
	char *path;
	
	if ((path = argv[optind]) == NULL) {
		error("no argument supplied for infile");
		exit(EXIT_FAILURE);
	}
	
	shmAllocate(true);
	semAllocate(true);
	
	char msg[BUFFER_LENGTH];
	strfcpy(msg, "%s: Starting with options: s=%d, t=%d\n", ftime(), s, t);
	fprintf(stderr, msg);
	flog("output.log", msg);
	
	int c = load(path);
	
	n = MIN(c, CHILD_COUNT);
	s = MIN(s, n);
	
	if (t == 0) {
		finalize(true);
		exit(EXIT_SUCCESS);
	} else timer(t);
	
	int i = 0;
	int j = n;
	
	while (i < s)
		spawn(i++);
	
	int status;
	
	while (j > 0 && s > 0) {
		wait(&status);
		if (WIFEXITED(status)) flog("output.log", "%s: Process %d finished\n", ftime(), WEXITSTATUS(status));
		if (i < n) spawn(i++);
		j--;
	}
	
	finalize(true);
	
	return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}

static void usage(int status) {
	if (status != EXIT_SUCCESS) fprintf(stderr, "Try '%s -h' for more information\n", getProgramName());
	else {
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

static int load(char *path) {
	FILE *fp;
	if ((fp = fopen(path, "r")) == NULL) crash("fopen");
	
	int i = 0;
	char *line;
	size_t len = 0;
	ssize_t read;
	while (i < n && (read = getline(&line, &len, fp)) != -1) {
		crnl(line);
		setString(i++, line);
	}
	
	if (fclose(fp) == EOF) crash("fclose");
	if (line) free(line);
	
	return i;
}

static void spawn(int index) {
	int cpid = fork();
	if (cpid == -1) crash("fork");
	else if (cpid == 0) {
		if (index == 0) setCpgid(getpid());
		setpgid(0, getCpgid());
		flog("output.log", "%s: Process %d starting\n", ftime(), index);
		char cindex[3];
		sprintf(cindex, "%d", index);
		execl("./palin", "palin", cindex, (char*) NULL);
		exit(EXIT_SUCCESS);
	}
}

static void timer(int seconds) {
	sigact(SIGALRM, &handler);
	
	struct itimerval itv;
	itv.it_value.tv_sec = seconds;
	itv.it_value.tv_usec = 0;
	itv.it_interval.tv_sec = 0;
	itv.it_interval.tv_usec = 0;
	if (setitimer(ITIMER_REAL, &itv, NULL) == -1) crash("setitimer");
}

static void handler(int signum) {
	char msg[BUFFER_LENGTH];
	strfcpy(msg, "%s: Signal %s caught\n", ftime(), signum == SIGALRM ? "timeout" : "interrupt");
	fprintf(stderr, msg);
	flog("output.log", msg);
	killpg(getCpgid(), signum == SIGALRM ? SIGUSR1 : SIGTERM);
	while (wait(NULL) > 0);
	finalize(false);
	exit(EXIT_SUCCESS);
}

static void finalize(bool finished) {
	char msg[BUFFER_LENGTH];
	strfcpy(msg, "%s: Exiting %sfinished\n", ftime(), finished ? "" : "un");
	fprintf(stderr, msg);
	flog("output.log", msg);
	shmRelease();
	semRelease();
}
