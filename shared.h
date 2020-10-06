/*
 * Author: Jared Diehl
 * Date: October 4, 2020
 */

#ifndef SHARED_H
#define SHARED_H

#include <sys/sem.h> // sembuf
#include <sys/types.h> // key_t

#include "constant.h"

struct shm {
	char strings[STRING_COUNT][STRING_LENGTH];
};

char *programName;

key_t shmkey;
int shmid;
struct shm *shmptr;

key_t semkey;
int semid;
struct sembuf sop;

void init(int, char**);

void shmAllocate();
void shmRelease();
char *getString(int);

void semAllocate();
void semRelease();
void semWait();
void semSignal();

#endif
