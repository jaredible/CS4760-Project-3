/*
 * palin.c 10/16/20
 * Jared Diehl (jmddnb@umsystem.edu)
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 * This program checks to see if a
 * string is a palindrome, and outputs
 * the results.
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
	
	/* Register signal handler */
	sigact(SIGTERM, handler); /* Used for handling Ctrl+C signal from master */
	sigact(SIGUSR1, handler); /* Used for handling timeout signal from master */
	
	/* Check and get option */
	if (argc < 2) {
		error("no argument supplied for index"); /* Error if we don't get the index argument */
		exit(EXIT_FAILURE);
	} else cindex = atoi(argv[1]); /* Set this process' index to the second argument */
	
	/* Initialize random's seed */
	srand(time(NULL) + getpid() * cindex); /* Using time, PID, and this process' index increases randomness */
	
	/* Attach to shared memory and semaphores */
	shmAllocate(false); /* Allocate the shared memory, but don't initialize it */
	semAllocate(false); /* Allocate the semaphores, but don't initialize them */
	
	/* Check if string is a palindrome */
	char *string = getString(cindex); /* Get the string using this process' index */
	bool is = palindrome(string); /* Get if the string is a palindrome */
	
	if (DEBUG) printf("%s: Process %d found that %s is %sa palindrome\n", ftime(), cindex, string, is ? "" : "not ");
	
	/* Display that this process wants to enter the critical section */
	fprintf(stderr, "%s: Process %d wants to enter critical section\n", ftime(), cindex);
	
	/* Locks a semaphore */
	semWait(is ? 0 : 1); /* 0 is for palin.out, 1 is for nopalin.out */
	
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
	
	/* Unlocks a semaphore */
	semSignal(is ? 0 : 1); /* 0 is for palin.out, 1 is for nopalin.out */
	
	/* Exit with the process' index */
	return cindex + EXIT_STATUS_OFFSET; /* Used for master to know which process has finished */
}

/*
 * palindrome(string)
 * ------------------
 * Returns if the given string is a palindrome.
 */
static bool palindrome(char *string) {
	int li, ri;
	char lc, rc;
	
	/* Loop until ri > li */
	for (li = 0, ri = strlen(string) - 1; ri > li; li++, ri--) {
		/* Increment li until lc is an alphanumeric character */
		while (!isalnum((lc = tolower(string[li]))))
			li++;
		/* Decrement ri until rc is an alphanumeric character */
		while (!isalnum((rc = tolower(string[ri]))))
			ri--;
		
		/* Since lc is not rc, the characters are different, so the string isn't a palindrome */
		if (lc != rc) return false;
	}
	
	/* So the string is a palindrome */
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
