#include <errno.h>
#include <limits.h>
#include <linux/limits.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <unistd.h>

#include "utils.h"

#ifndef SHELL_H
#define SHELL_H

bool startsWith(const char *str, const char *substr) {
    return (strncmp(str, substr, strlen(substr)) == 0);
}

#endif