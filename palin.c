/*
 * Author: Jared Diehl
 * Date: October 4, 2020
 */

#include <ctype.h> // tolower
#include <signal.h>
#include <stdbool.h> // false, true
#include <stdio.h> // printf
#include <stdlib.h> // EXIT_SUCCESS
#include <string.h> // strlen
#include <sys/sem.h>
#include <time.h> // time
#include <unistd.h> // sleep

#include "constant.h"
#include "helper.h"
#include "shared.h"

static bool isPalindrome(char*);
static void exitHandler(int);

static int idx;

int main(int argc, char** argv) {
	init(argc, argv);
	
	sigact(SIGTERM, &exitHandler);
	sigact(SIGUSR1, &exitHandler);
	
//	printf("pid: %d, ppid: %d, pgid: %d\n", getpid(), getppid(), getpgid(getpid()));
	
	if (argc < 2) {
		error("no argument supplied for index");
	} else {
		idx = atoi(argv[1]);
	}
	
	srand(time(NULL) + idx);
	
	shmAllocate(false);
	semAllocate(false);
	
	char *string = getString(idx);
	bool isp = isPalindrome(string);
	
	fprintf(stderr, "%s: Process %d wants to enter critical section\n", ftime(), idx);
	
	semWait();
	
	/* Enter critical section */
	
	fprintf(stderr, "%s: Process %d in critical section\n", ftime(), idx);
	sleep(rand() % 3);
	flog(isp ? "palin.out" : "nopalin.out", "%s %d %d %s\n", ftime(), getpid(), idx, string);
	fprintf(stderr, "%s: Process %d exiting critical section\n", ftime(), idx);
	
	/* Exit critical section */
	
	semSignal();
	
	return EXIT_SUCCESS;
}

static bool isPalindrome(char *string) {
	int leftIndex = 0, rightIndex = strlen(string) - 1;
	char leftChar, rightChar;
	
	while (rightIndex > leftIndex) {
		leftChar = tolower(string[leftIndex]);
		rightChar = tolower(string[rightIndex]);
		
		if (leftChar != rightChar) return false;
		
		leftIndex++;
		rightIndex--;
	}
	
	return true;
}

static void exitHandler(int signum) {
	if (signum == SIGTERM || signum == SIGUSR1) {
		char msg[BUFFER_LENGTH];
		strfcpy(msg, "%s: Process %d exiting due to %s signal\n", ftime(), idx, signum == SIGUSR1 ? "timeout" : "interrupt");
		fprintf(stderr, msg);
		flog("output.log", msg);
		exit(EXIT_FAILURE);
	}
}
