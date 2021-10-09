#ifndef BUILTIN_H
#define BUILTIN_H

#include "header.h"
#include "utils.h"
#include "fg_bg.h"

int pwd();
int echo(char *commandName, char delimiters[], char *saveptr);
int cd(char **saveptr, char homeDirectory[], char previousDirectory[]);

char get_status(pid_t pid);
int pinfo(char *homeDirectory, char *saveptr);

#endif