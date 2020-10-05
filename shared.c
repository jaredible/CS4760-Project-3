/*
 * Author: Jared Diehl
 * Date: October 4, 2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "constant.h"
#include "shared.h"

void init(int argc, char **argv) {
	programName = argv[0];
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
}

void allocateMemory() {
	if ((shmKey = ftok(".", 0)) == -1) {
		perror("ftok");
		exit(EXIT_FAILURE);
	}
	if ((shmSegmentId = shmget(shmKey, sizeof(struct shm), PERMS | IPC_CREAT)) == -1) {
		perror("shmget");
		exit(EXIT_FAILURE);
	} else {
		shmptr = (struct shm*) shmat(shmSegmentId, NULL, 0);
	}
}

void releaseMemory() {
	if (shmptr != NULL) {
		if (shmdt(shmptr) == -1) {
			perror("shmdt");
			exit(EXIT_FAILURE);
		}
	}
	if (shmSegmentId > 0) {
		if (shmctl(shmSegmentId, IPC_RMID, NULL) == -1) {
			perror("shmctl");
			exit(EXIT_FAILURE);
		}
	}
}
