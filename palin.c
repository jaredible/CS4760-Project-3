/*
 * Author: Jared Diehl
 * Date: October 4, 2020
 */

#include <ctype.h> // tolower
#include <stdbool.h> // false, true
#include <stdio.h> // printf
#include <stdlib.h> // EXIT_SUCCESS
#include <string.h> // strlen

bool isPalindrome(char*);

int main(int argc, char** argv) {
	char *strings[] = {"a", "a0A", "ab0", "", "aBbA", "ab"};
	
	int i = 0;
	for (; i < 6; i++) {
		char *string = strings[i];
		printf("string: %s, isPalindrome: %s\n", string, isPalindrome(string) ? "true" : "false");
	}
	
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
