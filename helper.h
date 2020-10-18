/*
 * helper.h 10/16/20
 * Jared Diehl (jmddnb@umsystem.edu)
 */

#ifndef HELPER_H
#define HELPER_H

#define BUFFER_LENGTH 1024

#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

void sigact(int, void(int));
void crnl(char*);
void flog(char*, char*, ...);
char *ftime();
void rtouch(char*);

#endif
