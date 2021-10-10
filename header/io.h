#ifndef IO_H
#define IO_H

#include "header.h"
#include "utils.h"

#define APPEND 0
#define TRUNCATE 1

int removeAndSetIO(char *token, char *outputFilePath, char *inputFilePath, int *outputMode);

void resetIO();

bool isPipe(const char *token);

#endif