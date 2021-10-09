#include "header.h"
#include "utils.h"
#include "builtin.h"
#include "history.h"
#include "ls.h"
#include "fg_bg.h"
#include "vector.h"

#ifndef SHELL_H
#define SHELL_H

void setup();
int execute(char *token, char homeDirectory[], char previousDirectory[]);
void add_bg(Command command);

#endif