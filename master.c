/*
 * Author: Jared Diehl
 * Date: October, 4, 2020
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

void usage(int);
int load(char*);
void spawn(int);
void timer(int);
void exitHandler(int);

int n = TOTAL_CHILDREN_DEFAULT;
int s = CONCURRENT_CHILDREN_DEFAULT;
int t = TIMEOUT_DEFAULT;

int main(int argc, char** argv) {
	init(argc, argv);
	
	sigact(SIGINT, &exitHandler);
	
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
	
	if (!ok) {
		usage(EXIT_FAILURE);
	}
	
	char *path;
	
	if ((path = argv[optind]) == NULL) {
		error("no argument supplied for infile");
		usage(EXIT_FAILURE);
	}
	
	shmAllocate();
	semAllocate();
	
	int c = load(path);
	
	n = MIN(c, CHILD_COUNT);
	s = MIN(s, n);
	
	timer(t);
	
	int i = 0;
	int j = n;
	
	while (i < s) {
		spawn(i++);
	}
	
	while (j > 0) {
		wait(NULL);
		flog("output.log", "%s: Process %d finished\n", ftime(), n - j);
		if (i < n) {
			spawn(i++);
		}
		j--;
	}
	
	shmRelease();
	semRelease();
	
	return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}

void usage(int status) {
	if (status != EXIT_SUCCESS) {
		fprintf(stderr, "Try '%s -h' for more information\n", programName);
	} else {
		printf("NAME\n");
		printf("       %s - palindrome finder\n", programName);
		printf("USAGE\n");
		printf("       %s -h\n", programName);
		printf("       %s [-s x] [-t time] infile\n", programName);
		printf("DESCRIPTION\n");
		printf("       -h       : Print a help message or usage, and exit\n");
		printf("       -s x     : Number of children allowed to exist concurrently (default 2)\n");
		printf("       -t time  : Time, in seconds, after which the program will terminate (default 100)\n");
	}
	exit(status);
}

int load(char *path) {
	FILE *fp;
	if ((fp = fopen(path, "r")) == NULL) {
		perror("fopen");
		exit(EXIT_FAILURE);
	}
	
	int i = 0;
	char *line;
	size_t len = 0;
	ssize_t read;
	while (i < n && (read = getline(&line, &len, fp)) != -1) {
		crnl(line);
		strcpy(shmptr->strings[i++], line);
	}

	fclose(fp);
	if (line) free(line);

	return i;
}

void spawn(int index) {
	pid_t pid = fork();
	if (pid == -1) {
		perror("fork");
		exit(EXIT_FAILURE);
	} else if (pid == 0) {
		flog("output.log", "%s: Process %d starting\n", ftime(), index);
		char cindex[3];
		sprintf(cindex, "%d", index);
		execl("./palin", "palin", cindex, (char*) NULL);
		exit(EXIT_SUCCESS);
	}
}

void timer(int seconds) {
	sigact(SIGALRM, &exitHandler);
	
	struct itimerval itv;
	itv.it_value.tv_sec = seconds;
	itv.it_value.tv_usec = 0;
	itv.it_interval.tv_sec = 0;
	itv.it_interval.tv_usec = 0;
	if (setitimer(ITIMER_REAL, &itv, NULL) == -1) {
		perror("setitimer");
		exit(EXIT_FAILURE);
	}
}

void exitHandler(int signum) {
	sigact(SIGTERM, SIG_IGN);
	kill(-getpid(), SIGTERM);
	while (wait(NULL) > 0);
	shmRelease();
	semRelease();
	exit(EXIT_SUCCESS);
}
