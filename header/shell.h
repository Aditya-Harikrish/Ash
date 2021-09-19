#include "header.h"
#include "utils.h"
#include "builtin.h"
#include "history.h"
#include "ls.h"

#ifndef SHELL_H
#define SHELL_H

#define MAX_COMMAND_SIZE 5000

int execute(char *token, char homeDirectory[], char previousDirectory[]);

#endif