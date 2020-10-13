/*
 * helper.c 10/16/20
 * Jared Diehl (jmddnb@umsystem.edu)
 */

#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#include "helper.h"

void sigact(int signum, void handler(int)) {
	struct sigaction sa;
	if (sigemptyset(&sa.sa_mask) == -1) crash("sigemptyset");
	sa.sa_handler = handler;
	sa.sa_flags = 0;
	if (sigaction(signum, &sa, NULL) == -1) crash("sigaction");
}

void crnl(char *s) {
	for (; *s; s++)
		if (*s == '\n') *s = '\0';
}

void flog(char *path, char *fmt, ...) {
	FILE *fp;
	if ((fp = fopen(path, "a+")) == NULL) crash("fopen");
	
	char buf[BUFFER_LENGTH];
	va_list args;
	
	va_start(args, fmt);
	vsnprintf(buf, BUFFER_LENGTH, fmt, args);
	va_end(args);
	
	fprintf(fp, buf);
	fclose(fp);
}

char *ftime() {
	char *ftime = malloc(BUFFER_LENGTH * sizeof(char));
	time_t now = time(NULL);
	strftime(ftime, BUFFER_LENGTH, "%H:%M:%S", localtime(&now));
	return ftime;
}

void rtouch(char *path) {
	FILE *fp;
	if ((fp = fopen(path, "w")) == NULL) crash("fopen");
	if (fclose(fp) == EOF) crash("fclose");
}
