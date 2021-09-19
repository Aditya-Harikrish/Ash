#ifndef VECTOR_H
#define VECTOR_H

#include "header.h"
#include "utils.h"

typedef struct {
    char **arr;
    size_t size, capacity;
} SVector;

void init_vector(SVector *v);
int pushback(SVector *v, char *ele);
void print_vector(SVector v);
int popback(SVector *v);
void delete_vector(SVector *v);

#endif