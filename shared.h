/*
 * Author: Jared Diehl
 * Date: October 16, 2020
 */

#ifndef SHARED_H
#define SHARED_H

#include <stdbool.h> // bool
#include <sys/types.h> // pid_t

void init(int, char**);
char *getProgramName();

void shmAllocate(bool);
void shmRelease();
char *getString(int);
void setString(int, char*);
pid_t getChildProcessGroupId();
void setChildProcessGroupId(pid_t);

void semAllocate(bool);
void semRelease();
void semWait();
void semSignal();

#endif
