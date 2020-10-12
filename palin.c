/*
 * palin.c 10/16/20
 * Jared Diehl (jmddnb@umsystem.edu)
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

static bool palindrome(char*);
static void handler(int);

static int cindex;

int main(int argc, char** argv) {
	init(argc, argv);
	
	sigact(SIGTERM, &handler);
	sigact(SIGUSR1, &handler);
	
	if (argc < 2) error("no argument supplied for index");
	else cindex = atoi(argv[1]);
	
	srand(time(NULL) + getpid() * cindex);
	
	shmAllocate(false);
	semAllocate(false);
	
	char *string = getString(cindex);
	bool is = palindrome(string);

	if (DEBUG) printf("%s: Process %d found that %s is %sa palindrome\n", ftime(), cindex, string, is ? "" : "not ");
	
	fprintf(stderr, "%s: Process %d wants to enter critical section\n", ftime(), cindex);
	
	semWait(is ? 0 : 1);
	
	/* Enter critical section */
	
	fprintf(stderr, "%s: Process %d in critical section\n", ftime(), cindex);
	sleep(rand() % 3);
	flog(is ? "palin.out" : "nopalin.out", "%s %d %d %s\n", ftime(), getpid(), cindex, string);
	fprintf(stderr, "%s: Process %d exiting critical section\n", ftime(), cindex);
	
	/* Exit critical section */
	
	semSignal(is ? 0 : 1);
	
	return cindex;
}


static bool palindrome(char *string) {
	int li, ri;
	char lc, rc;
	
	for (li = 0, ri = strlen(string) - 1; ri > li; li++, ri--) {
		while (!isalnum((lc = tolower(string[li]))))
			li++;
		while (!isalnum((rc = tolower(string[ri]))))
			ri--;
		
		if (lc != rc) return false;
	}
	
	return true;
}

static void handler(int signum) {
	semWait(2);
	char msg[BUFFER_LENGTH];
	strfcpy(msg, "%s: Process %d exiting due to %s signal\n", ftime(), cindex, signum == SIGUSR1 ? "timeout" : "interrupt");
	fprintf(stderr, msg);
	flog("output.log", msg);
	semSignal(2);
	exit(EXIT_FAILURE);
}
