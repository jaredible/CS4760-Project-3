/*
 * Author: Jared Diehl
 * Date: October 4, 2020
 */

#ifndef SHARED_H
#define SHARED_H

#include <sys/types.h> // key_t

struct shm {
	char strings[20][256];
};

char *programName;

key_t shmKey;
int shmSegmentId;
struct shm *shmptr;

void init(int, char**);

void allocateMemory();
void releaseMemory();

#endif
