/*
 * Author: Jared Diehl
 * Date: October, 4, 2020
 */

#include <ctype.h> // isdigit
#include <getopt.h> // getopt, optarg, optind
#include <stdarg.h> // va_end, va_list, va_start
#include <stdbool.h> // false, true
#include <stdio.h> // fprintf, perror, printf, stderr, vsnprintf
#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS, atoi, exit
#include <string.h> // strcpy
#include <sys/ipc.h> // IPC_CREAT, IPC_RMID, ftok
#include <sys/shm.h> // shmat, shmctl, shmdt, shmget
#include <sys/stat.h> // S_IRUSR, S_IWUSR

#define PERMS (S_IRUSR | S_IWUSR)

struct shm {
	char strings[20][256];
};

void usage(int);
void error(char*, ...);
void allocateMemory();
void releaseMemory();

char *programName = NULL;

int s = 2;
int t = 100;

int shmKey;
int shmSegmentId;
struct shm *shmptr;

int main(int argc, char** argv) {
	programName = argv[0];
	
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

void allocateMemory() {
	if ((shmKey = ftok(".", 0)) == -1) {
		perror("ftok");
		exit(EXIT_FAILURE);
	}
	
	if ((shmSegmentId = shmget(shmKey, sizeof(struct shm), PERMS | IPC_CREAT)) == -1) {
		perror("shmget");
		exit(EXIT_FAILURE);
	} else {
		shmptr = (struct shm*) shmat(shmSegmentId, NULL, 0);
	}
}

void releaseMemory() {
	if (shmptr != NULL) {
		if (shmdt(shmptr) == -1) {
			perror("shmdt");
			exit(EXIT_FAILURE);
		}
	}
	if (shmSegmentId > 0) {
		if (shmctl(shmSegmentId, IPC_RMID, NULL) == -1) {
			perror("shmctl");
			exit(EXIT_FAILURE);
		}
	}
}
