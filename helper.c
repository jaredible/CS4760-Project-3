/*
 * Author: Jared Diehl
 * Date: October 16, 2020
 */

#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#include "helper.h"
#include "shared.h"

void error(char *fmt, ...) {
	char buf[BUFFER_LENGTH];
	va_list args;
	
	va_start(args, fmt);
	vsnprintf(buf, BUFFER_LENGTH, fmt, args);
	va_end(args);
	
	fprintf(stderr, "%s: %s\n", programName, buf);
}

void sigact(int signum, void handler(int)) {
	struct sigaction sa;
	if (sigemptyset(&sa.sa_mask) == -1) {
		crash("sigemptyset");
	}
	sa.sa_handler = handler;
	sa.sa_flags = SA_RESTART;
	if (sigaction(signum, &sa, NULL) == -1) {
		crash("sigaction");
	}
}

void crnl(char *s) {
	while (*s) {
		if (*s == '\n') {
			*s = '\0';
		}
		s++;
	}
}

void flog(char *path, char *fmt, ...) {
	FILE *fp;
	if ((fp = fopen(path, "a+")) == NULL) {
		crash("fopen");
	}
	
	char buf[BUFFER_LENGTH];
	va_list args;
	
	va_start(args, fmt);
	vsnprintf(buf, BUFFER_LENGTH, fmt, args);
	va_end(args);
	
	fprintf(fp, buf);
	fclose(fp);
}

char *ftime() {
	int n = 100;
	char *ftime = malloc(n * sizeof(char));
	time_t now = time(NULL);
	strftime(ftime, n, "%H:%M:%S", localtime(&now));
	return ftime;
}

void rtouch(char *path) {
	FILE *fp;
	if ((fp = fopen(path, "w")) == NULL) {
		crash("fopen");
	}
	fclose(fp);
}

void crash(char *msg) {
	char buf[BUFFER_LENGTH];
	snprintf(buf, BUFFER_LENGTH, "%s: %s", programName, msg);
	perror(buf);
	exit(EXIT_FAILURE);
}

void strfcpy(char *src, char *fmt, ...) {
	char buf[BUFFER_LENGTH];
	va_list args;
	
	va_start(args, fmt);
	vsnprintf(buf, BUFFER_LENGTH, fmt, args);
	va_end(args);
	
	strncpy(src, buf, BUFFER_LENGTH);
}
