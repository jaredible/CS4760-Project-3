/*
 * palin.c 10/16/20
 * Jared Diehl (jmddnb@umsystem.edu)
 */

/* Standard library header files */
#include <ctype.h> // tolower
#include <signal.h> // SIGTERM, SIGUSR1
#include <stdbool.h> // false, true
#include <stdio.h> // printf
#include <stdlib.h> // EXIT_SUCCESS
#include <string.h> // strlen
#include <time.h> // time
#include <unistd.h> // sleep

/* Programmer-defined header files */
#include "constant.h"
#include "helper.h"
#include "shared.h"

/* Private function prototypes */
static bool palindrome(char*);
static void handler(int);

/* Private global variables */
static int cindex;

/* Main program */
int main(int argc, char **argv) {
	init(argc, argv);
	
	/* Register signal handlers */
	sigact(SIGTERM, &handler);
	sigact(SIGUSR1, &handler);
	
	/* Check and get option */
	if (argc < 2) error("no argument supplied for index");
	else cindex = atoi(argv[1]);
	
	/* Initialize random's seed */
	srand(time(NULL) + getpid() * cindex);
	
	/* Attach to shared memory and semaphores */
	shmAllocate(false);
	semAllocate(false);
	
	/* Check if string is a palindrome */
	char *string = getString(cindex);
	bool is = palindrome(string);
	
	if (DEBUG) printf("%s: Process %d found that %s is %sa palindrome\n", ftime(), cindex, string, is ? "" : "not ");
	
	/* Display that this process wants to enter the critical section */
	fprintf(stderr, "%s: Process %d wants to enter critical section\n", ftime(), cindex);
	
	/* Lock semaphore */
	semWait(is ? 0 : 1);
	
	/* Enter critical section */
	
	/* Display that this process is in the critical section */
	fprintf(stderr, "%s: Process %d in critical section\n", ftime(), cindex);
	/* Sleep for 0-2 seconds */
	sleep(rand() % 3);
	/* Output the string to the appropriate output file */
	flog(is ? "palin.out" : "nopalin.out", "%s %d %d %s\n", ftime(), getpid(), cindex, string);
	/* Display that this process is exiting the critical section */
	fprintf(stderr, "%s: Process %d exiting critical section\n", ftime(), cindex);
	
	/* Exit critical section */
	
	/* Unlock semaphore */
	semSignal(is ? 0 : 1);
	
	/* Exit with the process' index */
	return cindex;
}

/*
 * palindrome(string)
 * ------------------
 * Returns if the given string is a palindrome.
 */
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

/*
 * handler(signal)
 * ---------------
 * Handles all programmer-defined signals.
 */
static void handler(int signal) {
	/* Output the caught status and exit */
	semWait(2);
	char msg[BUFFER_LENGTH];
	sprintf(msg, "%s: Process %d exiting due to %s signal\n", ftime(), cindex, signal == SIGUSR1 ? "timeout" : "interrupt");
	fprintf(stderr, msg);
	flog("output.log", msg);
	semSignal(2);

	exit(EXIT_FAILURE);
}
