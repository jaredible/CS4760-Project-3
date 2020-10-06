/*
 * Author: Jared Diehl
 * Date: October 4, 2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "constant.h"
#include "helper.h"
#include "shared.h"

void init(int argc, char **argv) {
	programName = argv[0];
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
}

void shmAllocate() {
	if ((shmkey = ftok(KEY_PATHNAME, KEY_PROJID)) == -1) {
		crash("ftok");
	}
	if ((shmid = shmget(shmkey, sizeof(struct shm), PERMS | IPC_CREAT)) == -1) {
		crash("shmget");
	} else {
		shmptr = (struct shm*) shmat(shmid, NULL, 0);
	}
}

void shmRelease() {
	if (shmptr != NULL) {
		if (shmdt(shmptr) == -1) {
			crash("shmdt");
		}
	}
	if (shmid > 0) {
		if (shmctl(shmid, IPC_RMID, NULL) == -1) {
			crash("shmctl");
		}
	}
}

char *getString(int index) {
	return shmptr->strings[index];
}

void setString(int index, char *string) {
	strcpy(shmptr->strings[index], string);
}

pid_t getChildProcessGroupId() {
	return shmptr->cpgid;
}

void setChildProcessGroupId(pid_t cpgid) {
	shmptr->cpgid = cpgid;
}

void semAllocate() {
	if ((semkey = ftok(KEY_PATHNAME, KEY_PROJID)) == -1) {
		crash("ftok");
	}
	if ((semid = semget(semkey, 1, PERMS | IPC_CREAT)) == -1) {
		crash("semget");
	}
	semctl(semid, 0, SETVAL, 1);
}

void semRelease() {
	semctl(semid, 0, IPC_RMID);
}

void semWait() {
	sop.sem_num = 0;
	sop.sem_op = -1;
	sop.sem_flg = 0;
	if (semop(semid, &sop, 1) == -1) {
		crash("semop");
	}
}

void semSignal() {
	sop.sem_num = 0;
	sop.sem_op = 1;
	sop.sem_flg = 0;
	if (semop(semid, &sop, 1) == -1) {
		crash("semop");
	}
}
