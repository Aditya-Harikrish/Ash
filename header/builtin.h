#ifndef BUILTIN_H
#define BUILTIN_H

#include "header.h"
#include "utils.h"

int pwd();
int echo(char *commandName, char delimiters[], char *saveptr);
int cd(char **saveptr, char homeDirectory[], char previousDirectory[]);

#endif