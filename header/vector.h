#ifndef VECTOR_H
#define VECTOR_H

#include "header.h"
#include "utils.h"

typedef struct {
    char **arr;
    size_t size, capacity;
} SVector;

void S_init_vector(SVector *v);
int S_pushback(SVector *v, char *ele);
void S_print_vector(SVector v);
int S_popback(SVector *v);
void S_delete_vector(SVector *v);

/* Returns a pointer to the first match if found. Otherwise returns NULL */
char *S_find(SVector v, char *target);

typedef struct {
    char command[MAX_COMMAND_SIZE], commandName[MAX_COMMAND_SIZE];
    pid_t pid;
    int job_number;
} Command;

typedef struct {
    Command *arr;
    size_t size, capacity;
} CVector; // Vector of Commands, for bg processes

void C_init_vector(CVector *v);
int C_pushback(CVector *v, Command ele);
void C_print_vector(CVector v);
int C_popback(CVector *v);
void C_delete_vector(CVector *v);

/* Returns a pointer to the first match if found. Otherwise returns NULL */
Command *C_find(CVector v, pid_t pid);
bool C_is_equal(const Command *a, const Command *b);
int C_remove(CVector *v, pid_t pid);
#endif