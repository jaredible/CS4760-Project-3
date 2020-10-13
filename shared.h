/*
 * shared.h 10/16/20
 * Jared Diehl (jmddnb@umsystem.edu)
 */

#ifndef SHARED_H
#define SHARED_H

#include <stdbool.h> // bool
#include <sys/types.h>

void init(int, char**);
char *getProgramName();
void error(char*, ...);
void crash(char*);

void shmAllocate(bool);
void shmRelease();
char *getString(int);
void setString(int, char*);
void setCpgid(pid_t);
pid_t getCpgid();

void semAllocate(bool, ...);
void semRelease();
void semWait(int);
void semSignal(int);

#endif
