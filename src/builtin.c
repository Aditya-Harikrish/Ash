#include "builtin.h"

int pwd() {
    char currentDirectory[PATH_MAX + 1];
    if(getcwd(currentDirectory, sizeof(currentDirectory)) == NULL) {
        perror("Could not get the present working directory. Error with getcwd()");
        return FATAL_ERROR;
    }
    printf("%s\n", currentDirectory);
    return NO_ERROR;
}

int echo(char *commandName, char delimiters[], char *saveptr) {
    char *arg = commandName;
    arg = strtok_r(NULL, delimiters, &saveptr);
    while(arg != NULL) {
        printf("%s ", arg);
        arg = strtok_r(NULL, delimiters, &saveptr);
    }
    printf("\n");
    return NO_ERROR;
}

int cd(char **saveptr, char homeDirectory[], char previousDirectory[]) {
    char *arg = strtok_r(NULL, "\n", saveptr);

    char currentDirectory[PATH_MAX + 1];
    if(getcwd(currentDirectory, sizeof(currentDirectory)) == NULL) {
        perror("Error with getcwd()");
        return FATAL_ERROR;
    }

    /* If no argument is provided, cd to the home directory */
    if(arg == NULL || !strcmp(arg, "~")) {
        if(chdir(homeDirectory) != 0) {
            perror("Could not cd to the home directory");
            return FATAL_ERROR;
        }
    }
    else if(strlen(arg) > PATH_MAX) {
        printf("Path given is larger than PATH_MAX and is thus invalid");
        return WARNING_ERROR;
    }
    else if(arg[0] == '~') {
        if(DEBUG) {
            printf("arg is %s\n", arg);
            printf("First letter of arg is ~\n");
        }
        char destPath[PATH_MAX + 1];
        strcpy(destPath, homeDirectory);
        unsigned long i = strlen(destPath), j = 1;
        while(j < strlen(arg)) {
            destPath[i] = arg[j];
            ++i; ++j;
        }
        destPath[i] = '\0';
        if(DEBUG) {
            printf("destPath is %s\n", destPath);
        }
        if(chdir(destPath) != 0) {
            printf("Could not cd to %s\n", destPath);
            perror("cd error");
            return FATAL_ERROR;
        }
    }
    else if(!strcmp(arg, "-")) {
        if(chdir(previousDirectory) != 0) {
            printf("Could not cd to the previous directory, %s\n", previousDirectory);
            perror("chdir() error");
            return FATAL_ERROR;
        }
    }
    else if(chdir(arg) != 0) {
        printf("Could not cd to %s\n", arg);
        perror("cd error");
        return WARNING_ERROR;
    }

    /* Store arg in previousDirectory if it isn't '-' */
    strcpy(previousDirectory, currentDirectory);
    return NO_ERROR;
}