/*
 * Author: Jared Diehl
 * Date: October 4, 2020
 */

#ifndef CONSTANT_H
#define CONSTANT_H

#include <sys/stat.h>

#define KEY_PATHNAME "."
#define KEY_PROJID_SHM 0
#define KEY_PROJID_SEM 1
#define PERMS (S_IRUSR | S_IWUSR)

#define CHILDREN_MAX 20

#define TOTAL_CHILDREN_DEFAULT CHILDREN_MAX
#define CONCURRENT_CHILDREN_DEFAULT 2
#define TIMEOUT_DEFAULT 100

#endif
