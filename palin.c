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
#include <unistd.h> // sleep

#include "helper.h"
#include "shared.h"

bool isPalindrome(char*);

int main(int argc, char** argv) {
	init(argc, argv);
	
	printf("index: %s\n", argv[1]);
	
	allocateMemory();
	
	sleep(1);
	
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
