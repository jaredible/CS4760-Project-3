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

#include "helper.h"
#include "shared.h"

bool isPalindrome(char*);

int main(int argc, char** argv) {
	init(argc, argv);
	
	int index;
	
	if (argc < 2) {
		error("no argument supplied for index");
	} else {
		index = atoi(argv[1]);
	}
	
	srand(time(NULL) + index);
	
	shmAllocate();
	semAllocate();
	
	char *string = shmptr->strings[index];
	bool is = isPalindrome(string);
	
	fprintf(stderr, "%s: Process %d wants to enter critical section\n", getFormattedTime(), index);
	
	semWait();
	
	/* Enter critical section */
	
	fprintf(stderr, "%s: Process %d in critical section\n", getFormattedTime(), index);
	sleep(rand() % 3);
	logOutput(is ? "palin.out" : "nopalin.out", "%s %d %d %s\n", getFormattedTime(), getpid(), index, string);
	fprintf(stderr, "%s: Process %d exiting critical section\n", getFormattedTime(), index);
	
	/* Exit critical section */
	
	semSignal();
	
	return EXIT_SUCCESS;
}

bool isPalindrome(char *string) {
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
