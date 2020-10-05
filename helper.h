#ifndef HELPER_H
#define HELPER_H

#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

void error(char*, ...);
void sigAction(int, void(int));
void removeNewline(char*);
void logOutput(char*, char*, ...);
char *getFormattedTime();

#endif
