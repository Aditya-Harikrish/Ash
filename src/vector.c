#include "vector.h"

void init_vector(SVector *v) {
    v->arr = NULL;
    v->size = 0;
    v->capacity = 0;
}

int pushback(SVector *v, char *ele) {
    if(v == NULL) {
        printf("v is NULL in pushback. Initialise v using init_vector first!\n");
        return FATAL_ERROR;
    }
    if(v->arr == NULL) {
        if(v->size != 0 || v->capacity != 0) {
            printf("Erro: vector not initialised properly.\n");
            return FATAL_ERROR;
        }
        v->arr = (char **)malloc(sizeof(char *) * 2);
        if(v->arr == 0) {
            printf("Failed to allocate memory\n");
            return FATAL_ERROR;
        }

        v->size = 0;
        v->capacity = 2;
    }
    if(v->size == v->capacity - 1) {
        v->arr = (char **)realloc(v->arr, 2 * (v->capacity) * sizeof(char *));

        if(v->arr == NULL) {
            printf("Failed to allocate memory\n");
            return FATAL_ERROR;
        }
        v->capacity = v->capacity * 2;
    }
    v->arr[v->size] = (char *)malloc(sizeof(ele) + 1);
    if(v->arr[v->size] == NULL) {
        printf("Failed to allocate memory\n");
        return FATAL_ERROR;
    }
    strcpy(v->arr[v->size], ele);
    ++(v->size);
    return NO_ERROR;
}

int popback(SVector *v) {
    if(v->size == 0) {
        printf("Underflow error! Cannot popback from an empty vector!\n");
        return FATAL_ERROR;
    }
    free(v->arr[v->size - 1]);
    --(v->size);
    if(v->size == 0) {
        free(v->arr);
        v->arr = NULL;
        v->capacity = 0;
    }
    else if(v->size * 2 <= v->capacity) {
        v->capacity = v->size;
        v->arr = (char **)realloc(v->arr, v->size * sizeof(char *));
        if(v->arr == NULL) {
            printf("Failed to allocate memory\n");
            return FATAL_ERROR;
        }
    }
    return NO_ERROR;
}
void delete_vector(SVector *v) {
    for(unsigned long i = 0; i < v->size; ++i) {
        free(v->arr[i]);
    }
    free(v->arr);
    v->arr = NULL;
    v->capacity = 0;
    v->size = 0;
}
void print_vector(SVector v) {
    if(v.arr == NULL) {
        return;
    }
    for(unsigned long i = 0; i < v.size; ++i) {
        printf("%s\n", v.arr[i]);
    }
    printf("\n");
}