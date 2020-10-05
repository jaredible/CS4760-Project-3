/*
 * Author: Jared Diehl
 * Date: October 16, 2020
 */

#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>

#include "helper.h"
#include "shared.h"

void error(char *fmt, ...) {
	int n = 100;
	char buf[n];
	va_list args;
	
	va_start(args, fmt);
	vsnprintf(buf, n, fmt, args);
	va_end(args);
	
	fprintf(stderr, "%s: %s\n", programName, buf);
}

void sigAction(int signum, void handler(int)) {
	struct sigaction sa;
	if (sigemptyset(&sa.sa_mask) == -1) {
		perror("sigemptyset");
		exit(EXIT_FAILURE);
	}
	sa.sa_handler = handler;
	sa.sa_flags = SA_RESTART;
	if (sigaction(signum, &sa, NULL) == -1) {
		perror("sigaction");
		exit(EXIT_FAILURE);
	}
}

void removeNewline(char *s) {
	while (*s) {
		if (*s == '\n') {
			*s = '\0';
		}
		s++;
	}
}

void logOutput(char *path, char *fmt, ...) {
	FILE *fp;
	if ((fp = fopen(path, "a+")) == NULL) {
		perror("fopen");
		exit(EXIT_FAILURE);
	}
	
	int n = 1024;
	char buf[n];
	va_list args;
	
	va_start(args, fmt);
	vsnprintf(buf, n, fmt, args);
	va_end(args);
	
	fprintf(fp, buf);
	fclose(fp);
}

char *getFormattedTime() {
	int n = 100;
	char *ftime = malloc(n * sizeof(char));
	time_t now = time(NULL);
	strftime(ftime, n, "%H:%M:%S", localtime(&now));
	return ftime;
}
