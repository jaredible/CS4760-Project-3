#ifndef HELPER_H
#define HELPER_H

#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

void error(char*, ...);
void sigact(int, void(int));
void crnl(char*);
void flog(char*, char*, ...);
char *ftime();
void rtouch(char*);
void crash(char*);
void strfcpy(char*, char*, ...);

#endif
