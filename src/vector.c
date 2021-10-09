#include "vector.h"

void S_init_vector(SVector *v) {
    v->arr = NULL;
    v->size = 0;
    v->capacity = 0;
}

int S_pushback(SVector *v, char *ele) {
    if(v == NULL) {
        printf("v is NULL in S_pushback. Allocate memory to v first!\n");
        return FATAL_ERROR;
    }
    if(v->arr == NULL) {
        if(v->size != 0 || v->capacity != 0) {
            printf("Error: vector not initialised properly.\n");
            return FATAL_ERROR;
        }
        v->arr = (char **)malloc(sizeof(char *));
        after_malloc_check(v->arr);
        v->size = 0;
        v->capacity = 1;
    }
    if(v->size == v->capacity) {
        if(DEBUG) {
            printf("Reallocating memory to the vector\n");
        }
        v->arr = (char **)realloc(v->arr, 2 * (v->capacity) * sizeof(char *));
        after_malloc_check(v->arr);
        v->capacity = v->capacity * 2;
    }
    v->arr[v->size] = strdup(ele);
    after_malloc_check(v->arr[v->size]);
    ++(v->size);
    if(DEBUG) {
        printf("SVector: ele: %s | ", ele);
        printf("size: %zu | capacity: %zu\n", v->size, v->capacity);
    }
    return NO_ERROR;
}

int S_popback(SVector *v) {
    if(v->size == 0) {
        printf("Underflow error! Cannot S_popback from an empty vector!\n");
        return FATAL_ERROR;
    }
    free(v->arr[v->size - 1]);
    --(v->size);
    if(v->size == 0) {
        S_delete_vector(v);
    }
    else if(v->size * 2 <= v->capacity) {
        v->capacity = v->size;
        v->arr = (char **)realloc(v->arr, v->size * sizeof(char *));
        after_malloc_check(v->arr);
    }
    if(DEBUG) {
        printf("size: %zu\ncapacity: %zu\n", v->size, v->capacity);
    }
    return NO_ERROR;
}

void S_delete_vector(SVector *v) {
    for(size_t i = 0; i < v->size; ++i) {
        free(v->arr[i]);
    }
    free(v->arr);
    v->arr = NULL;
    v->capacity = 0;
    v->size = 0;
    if(DEBUG) {
        perror("Deleted SVector");
    }
}

void S_print_vector(SVector v) {
    if(v.arr == NULL) {
        return;
    }
    if(DEBUG) {
        printf("Vector:\n");
    }
    for(size_t i = 0; i < v.size; ++i) {
        printf("%s\n", v.arr[i]);
    }
    printf("\n");
}

char *S_find(SVector v, char *target) {
    for(size_t i = 0; i < v.size; ++i) {
        if(!strcmp(v.arr[i], target)) {
            return v.arr[i];
        }
    }
    return NULL;
}

void C_init_vector(CVector *v) {
    v->arr = NULL;
    v->size = 0;
    v->capacity = 0;
}

int C_pushback(CVector *v, Command ele) {
    if(v == NULL) {
        printf("v is NULL in C_pushback. Allocate memory to v first!\n");
        return FATAL_ERROR;
    }
    if(v->arr == NULL) {
        if(v->size != 0 || v->capacity != 0) {
            printf("Error: vector not initialised properly.\n");
            return FATAL_ERROR;
        }
        v->arr = (Command *)malloc(sizeof(Command));
        after_malloc_check(v->arr);
        v->size = 0;
        v->capacity = 1;
    }
    if(v->size == v->capacity) {
        if(DEBUG) {
            printf("Reallocating memory to the vector\n");
        }
        v->arr = (Command *)realloc(v->arr, 2 * (v->capacity) * sizeof(Command));
        after_malloc_check(v->arr);
        v->capacity = v->capacity * 2;
    }
    v->arr[v->size] = ele;
    ++(v->size);
    if(DEBUG) {
        printf("CVector: size: %zu | capacity: %zu\n", v->size, v->capacity);
    }
    return NO_ERROR;
}

int C_popback(CVector *v) {
    if(v->size == 0) {
        printf("Underflow error! Cannot S_popback from an empty vector!\n");
        return FATAL_ERROR;
    }
    --(v->size);
    if(v->size == 0) {
        C_delete_vector(v);
    }
    else if(v->size * 2 <= v->capacity) {
        v->capacity = v->size;
        v->arr = (Command *)realloc(v->arr, v->size * sizeof(Command));
        after_malloc_check(v->arr);
    }
    if(DEBUG) {
        printf("CVector: size: %zu | capacity: %zu\n", v->size, v->capacity);
    }
    return NO_ERROR;
}

void C_delete_vector(CVector *v) {
    free(v->arr);
    v->arr = NULL;
    v->capacity = 0;
    v->size = 0;
    if(DEBUG) {
        perror("Deleted CVector");
    }
}

Command *C_find(CVector v, pid_t pid) {
    for(size_t i = 0; i < v.size; ++i) {
        if(v.arr[i].pid == pid) {
            return &(v.arr[i]);
        }
    }
    return NULL;
}

bool C_is_equal(const Command *a, const Command *b) {
    if(!strcmp(a->command, b->command) &&
    !strcmp(a->commandName, b->commandName) &&
    a->pid == b->pid &&
    a->job_number == b->job_number) {
        return true;
    }
    return false;
}

int C_remove(CVector *v, pid_t pid) {
    for(size_t i = 0; i < v->size; ++i) {
        if(v->arr[i].pid == pid) {
            for(size_t j = i; j < v->size - 1; ++j) {
                v->arr[j] = v->arr[j + 1];
            }
            --(v->size);
            return NO_ERROR;
        }
    }
    printf("Command with pid %d not found in bg_processes.\n", pid);
    return FATAL_ERROR;
}

void C_print_vector(CVector v) {
    printf("\nCVector:\n");
    for(size_t i = 0; i < v.size; ++i) {
        printf("%s | pid = %d | job number = %d\n", v.arr[i].command, v.arr[i].pid, v.arr[i].job_number);
    }
    printf("\n");
}


