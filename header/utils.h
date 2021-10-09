#ifndef UTILS_H
#define UTILS_H

#include "header.h"

pid_t pid_foreground;

#define CLEARSCREEN printf("\033[2J\033[1;1H")

#ifndef DEBUG
#define DEBUG 0
#endif

#define NO_ERROR        0
#define FATAL_ERROR     1
#define WARNING_ERROR   2
#define EXIT_PROGRAM    3

/* Defining Colours */
#define RED             "\x1b[31m"
#define GREEN           "\x1b[32m"
#define BRIGHT_GREEN    "\x1b[32m"
#define YELLOW          "\x1b[33m"
#define BLUE            "\x1b[34m"
#define BRIGHT_BLUE     "\x1b[94m"
#define MAGENTA         "\x1b[35m"
#define CYAN            "\x1b[36m"
#define RESET           "\x1b[0m"

#define MAX_NUMBER_OF_ARGS 100
#define MAX_COMMAND_SIZE 5000

#define after_malloc_check(x) \
if(x == NULL) { \
printf("Failed to allocate memory\n");\
return FATAL_ERROR;\
}

#define after_fork_check(pid) \
if(pid == -1) {\
    perror("fork()");\
    return FATAL_ERROR;\
}

/* Checks if str starts with substr */
bool startsWith(const char *str, const char *substr);

/* Remove leading and trailing whitespace */
void strip(char *str);

long min(long a, long b);
long max(long a, long b);

#endif