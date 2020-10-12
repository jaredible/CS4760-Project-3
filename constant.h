/*
 * constant.h 10/16/20
 * Jared Diehl (jmddnb@umsystem.edu)
 */

#ifndef CONSTANT_H
#define CONSTANT_H

#include <sys/stat.h>
#include <stdbool.h>

#define DEBUG false

#define KEY_PATHNAME "."
#define KEY_PROJID 'p'
#define PERMS (S_IRUSR | S_IWUSR)

#define STRING_COUNT 20
#define STRING_LENGTH 256

#define CHILD_COUNT STRING_COUNT

#define TOTAL_CHILDREN_DEFAULT STRING_COUNT
#define CONCURRENT_CHILDREN_DEFAULT 2
#define TIMEOUT_DEFAULT 100

#endif
