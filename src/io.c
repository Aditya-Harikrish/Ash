#include "io.h"

int removeAndSetIO(char *token, char *outputFilePath, char *inputFilePath, int *outputMode) {
    if(strlen(token) == 0) return NO_ERROR;
    if(outputFilePath == NULL || inputFilePath == NULL) {
        printf("Error: null string in removeAndSetIO()!\n");
        return FATAL_ERROR;
    }

#if DEBUG==1
    char saveToken[MAX_COMMAND_SIZE];
    strcpy(saveToken, token);
    fprintf(stderr, "Inside removeAndSetIO() at the moment\n");
#endif

    char whitespace[] = " \t\n\f\r\v";
    char newToken[MAX_COMMAND_SIZE];
    newToken[0] = '\0';
    char *saveptr = NULL;
    char *arg = strtok_r(token, whitespace, &saveptr);
    while(arg != NULL) {
        if(!strcmp(arg, "<") || !strcmp(arg, ">") || !strcmp(arg, ">>")) {
            if(arg == NULL) {
                printf("Error: file not found for redirection.\n");
                return WARNING_ERROR;
            }
            if(!strcmp(arg, ">")) {
                arg = strtok_r(NULL, whitespace, &saveptr);
                sprintf(outputFilePath, "%s", arg);
                *outputMode = TRUNCATE;
            }
            else if(!strcmp(arg, ">>")) {
                arg = strtok_r(NULL, whitespace, &saveptr);
                sprintf(outputFilePath, "%s", arg);
                *outputMode = APPEND;
            }
            else if(!strcmp(arg, "<")) {
                arg = strtok_r(NULL, whitespace, &saveptr);
                sprintf(inputFilePath, "%s", arg);
            }
            else {
                fprintf(stderr, "Error: no match with <, > and >>\n");
                printf("arg = %s\n", arg);
                return FATAL_ERROR;
            }
            arg = strtok_r(NULL, whitespace, &saveptr);
        }
        else {
            strcat(newToken, arg);
            strcat(newToken, " ");
            arg = strtok_r(NULL, whitespace, &saveptr);
        }
    }
    assert(newToken[strlen(newToken) - 1] == ' ');
    newToken[strlen(newToken) - 1] = '\n';
    strcpy(token, newToken);

#if DEBUG==1
    printf("strcmp(token, newToken) = %d\n", strcmp(saveToken, newToken));
    printf("outputFilePath: ");
    if(outputFilePath != NULL) {
        printf("%s", outputFilePath);
    }
    printf("\ninputFilePath: ");
    if(inputFilePath != NULL) {
        printf("%s", inputFilePath);
    }
    printf("\n");
    if(*outputMode != -1) {
        printf("outputMode = ");
        if(*outputMode == APPEND) printf("append\n");
        else if(*outputMode == TRUNCATE) printf("truncate\n");
    }
#endif

    if(outputFilePath != NULL && strlen(outputFilePath) > 0) {
        if(*outputMode == APPEND) {
            if(freopen(outputFilePath, "a+", stdout) == NULL) {
                printf("Error: could not open %s!\n", outputFilePath);
                perror("");
                return WARNING_ERROR;
            }
        }
        else if(*outputMode == TRUNCATE) {
            if(freopen(outputFilePath, "w+", stdout) == NULL) {
                printf("Error: could not open %s!\n", outputFilePath);
                perror("");
                return WARNING_ERROR;
            }
        }
    }

    if(inputFilePath != NULL && strlen(inputFilePath) > 0) {
        if(freopen(inputFilePath, "r", stdout) == NULL) {
            printf("Error: could not open %s!\n", inputFilePath);
            perror("");
            freopen("/dev/tty", "w", stdout);
            return WARNING_ERROR;
        }
    }
    if(DEBUG) {
        fprintf(stderr, "No error in removeAndSetIO()\n");
    }
    return NO_ERROR;
}

void resetIO() {
    freopen("/dev/tty", "w", stdout);
    freopen("/dev/tty", "r", stdin);
}

bool isPipe(const char *token) {
    for(size_t i = 0; i < strlen(token); ++i) {
        if(token[i] == '|') {
            return true;
        }
    }
    return false;
}





