#include "shell.h"

int execute(char *OGtoken, char homeDirectory[], char previousDirectory[]) {
    char token[MAX_COMMAND_SIZE];
    strcpy(token, OGtoken);
    if(DEBUG) {
        printf("Executing the command: %s\n", token);
    }
    char whitespace[] = " \t\n\f\r\v";
    char *saveptr = NULL;

    /* commandName is the name of the command, like cd and ls */
    char *commandName = strtok_r(token, whitespace, &saveptr);
    if(commandName == NULL) {
        return NO_ERROR;
    }

    if(DEBUG) {
        printf("commandName in execute(): %s\n", commandName);
    }

    if(!strcmp(commandName, "pwd")) {
        return pwd();
    }
    else if(!strcmp(commandName, "echo")) {
        return echo(commandName, whitespace, saveptr);
    }
    else if(!strcmp(commandName, "cd")) {
        return cd(&saveptr, homeDirectory, previousDirectory);
    }
    else if(!strcmp(commandName, "ls")) {
        return ls(saveptr, homeDirectory);

    }
    else if(!strcmp(commandName, "exit")) {
        return EXIT_PROGRAM;
    }
    else {
        printf("%s: command not found\n", commandName);
        return NO_ERROR;
    }
}