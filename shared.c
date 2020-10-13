/*
 * shared.c 10/16/20
 * Jared Diehl (jmddnb@umsystem.edu)
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#include "constant.h"
#include "helper.h"
#include "shared.h"

struct shm {
	char strings[STRING_COUNT][STRING_LENGTH];
	pid_t cpgid;
};

static char *programName = NULL;

static key_t shmkey;
static int shmid;
static struct shm *shmptr = NULL;

static key_t semkey;
static int semid;
static struct sembuf sop;

void init(int argc, char **argv) {
	programName = argv[0];
	
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
}

char *getProgramName() {
	return programName;
}

void error(char *fmt, ...) {
	char buf[BUFFER_LENGTH];
	va_list args;

	va_start(args, fmt);
	vsnprintf(buf, BUFFER_LENGTH, fmt, args);
	va_end(args);

	fprintf(stderr, "%s: %s\n", programName, buf);
}

void crash(char *msg) {
	char buf[BUFFER_LENGTH];
	snprintf(buf, BUFFER_LENGTH, "%s: %s", programName, msg);
	perror(buf);
	
	shmRelease();
	semRelease();

	exit(EXIT_FAILURE);
}

void shmAllocate(bool init) {
	if ((shmkey = ftok(KEY_PATHNAME, KEY_PROJID)) == -1) crash("ftok");
	if ((shmid = shmget(shmkey, sizeof(struct shm), PERMS | (init ? IPC_EXCL | IPC_CREAT : 0))) == -1) crash("shmget");
	else shmptr = (struct shm*) shmat(shmid, NULL, 0);
	if (init) flog("output.log", "%s: Shared memory allocated\n", ftime());
}

void shmRelease() {
	if (shmptr != NULL && shmdt(shmptr) == -1) crash("shmdt");
	if (shmid > 0) {
		if (shmctl(shmid, IPC_RMID, NULL) == -1) crash("shmctl");
		flog("output.log", "%s: Shared memory released\n", ftime());
	}
}

char *getString(int index) {
	return shmptr->strings[index];
}

void setString(int index, char *string) {
	strcpy(shmptr->strings[index], string);
}

void setCpgid(pid_t pid) {
	shmptr->cpgid = pid;
}

pid_t getCpgid() {
	return shmptr->cpgid;
}

void semAllocate(bool init, ...) {
	va_list args;
	int num = -1;
	va_start(args, init);
	num = va_arg(args, int);
	va_end(args);
	
	if ((semkey = ftok(KEY_PATHNAME, KEY_PROJID)) == -1) crash("ftok");
	if ((semid = semget(semkey, init ? num : 0, PERMS | (init ? IPC_EXCL | IPC_CREAT : 0))) == -1) crash("semget");
	if (init) {
		int i;
		for (i = 0; i < num; i++)
			if (semctl(semid, i, SETVAL, 1) == -1) crash("semctl");
		flog("output.log", "%s: Semaphore allocated\n", ftime());
	}
}

void semRelease() {
	if (semid > 0) {
		if (semctl(semid, 0, IPC_RMID) == -1) crash("semctl");
		flog("output.log", "%s: Semaphore released\n", ftime());
	}
}

void semWait(int num) {
	sop.sem_num = num;
	sop.sem_op = -1;
	sop.sem_flg = SEM_UNDO;
	if (semop(semid, &sop, 1) == -1) crash("semop");
}

void semSignal(int num) {
	sop.sem_num = num;
	sop.sem_op = 1;
	sop.sem_flg = SEM_UNDO;
	if (semop(semid, &sop, 1) == -1) crash("semop");
}
