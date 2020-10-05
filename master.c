/*
 * Author: Jared Diehl
 * Date: October, 4, 2020
 */

#include <ctype.h> // isdigit
#include <getopt.h> // getopt, optarg, optind
#include <stdarg.h> // va_end, va_list, va_start
#include <stdbool.h> // false, true
#include <stdio.h> // fprintf, perror, printf, sprintf, stderr, vsnprintf
#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS, atoi, exit
#include <string.h> // strcpy
#include <sys/ipc.h> // IPC_CREAT, IPC_RMID, ftok
#include <sys/shm.h> // shmat, shmctl, shmdt, shmget
#include <sys/stat.h> // S_IRUSR, S_IWUSR
#include <sys/types.h> // pid_t
#include <sys/wait.h> // wait
#include <unistd.h> // execl, fork

#include "constant.h"
#include "shared.h"

void usage(int);
void error(char*, ...);
void spawn(int);

int s = CONCURRENT_CHILDREN_DEFAULT;
int t = TIMEOUT_DEFAULT;

int main(int argc, char** argv) {
	init(argc, argv);
	
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
		error("missing input file");
		usage(EXIT_FAILURE);
	}
	
	printf("s: %d, t: %d, infile: %s\n", s, t, path);
	
	allocateMemory();
	
	strcpy(shmptr->strings[0], "test");
	printf("%s\n", shmptr->strings[0]);
	
	spawn(1);

	while(wait(NULL) > 0);

	releaseMemory();
	
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

void error(char *fmt, ...) {
	int n = 100;
	char buf[n];
	va_list args;
	
	va_start(args, fmt);
	vsnprintf(buf, n, fmt, args);
	va_end(args);
	
	fprintf(stderr, "%s: %s\n", programName, buf);
}

void spawn(int index) {
	pid_t pid = fork();
	if (pid == -1) {
		perror("fork");
		exit(EXIT_FAILURE);
	} else if (pid == 0) {
		char cindex[3];
		sprintf(cindex, "%d", index);
		execl("./palin", "palin", cindex, (char*) NULL);
		exit(EXIT_SUCCESS);
	}
}
