#include "shell.h"

void setup() {
    pid_foreground = -1;

    /* ctrl + C */
    struct sigaction ctrC_handler;

    ctrC_handler.sa_handler = ctrlC;
    sigemptyset(&ctrC_handler.sa_mask);
    ctrC_handler.sa_flags = 0;

    sigaction(SIGINT, &ctrC_handler, NULL);
}

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
    else if(!strcmp(commandName, "pinfo")) {
        return pinfo(homeDirectory, saveptr);
    }
    else if(!strcmp(commandName, "history")) {
        return history(saveptr);
    }
    else if(!strcmp(commandName, "exit")) {
        return EXIT_PROGRAM;
    }
    else if(!strcmp(commandName, "repeat")) {
        char *arg = strtok_r(NULL, whitespace, &saveptr);
        if(arg == NULL) {
            printf("Error: no argument provided\n");
            return WARNING_ERROR;
        }
        char *endptr = NULL;
        long num = strtol(arg, &endptr, 10);
        if(endptr == arg || errno == EINVAL || errno == ERANGE) {
            printf("Error: %s is not a valid number!\n", arg);
            perror("");
            return WARNING_ERROR;
        }
        char *restOfCommand = strtok_r(NULL, "\n", &saveptr);
        if(DEBUG) {
            printf("command to repeat %ld times : %s\n", num, restOfCommand);
        }
        for(long i = 0; i < num; ++i) {
            if(execute(restOfCommand, homeDirectory, previousDirectory) == FATAL_ERROR) {
                return FATAL_ERROR;
            }
        }
        return NO_ERROR;
    }
    else if(!strcmp(commandName, "jobs")) {
        return jobs(OGtoken, saveptr);
    }
    else if(!strcmp(commandName, "bg")) {
        return bg(saveptr);
    }
    else if(!strcmp(commandName, "sig")) {
        return sig(saveptr);
    }

    /* All other commands */
    else {
        bool background = false;
        char *arg = strtok_r(NULL, whitespace, &saveptr);
        while(arg != NULL) {
            if(!strcmp(arg, "&")) {
                background = true;
                break;
            }
            arg = strtok_r(NULL, whitespace, &saveptr);
        }
        char *copyOfToken = strdup(OGtoken);
        if(background) {
            return background_command(copyOfToken, commandName);
        }

        /* Foreground */
        else {
            return foreground_command(copyOfToken, commandName);
        }
        free(copyOfToken);
        return NO_ERROR;
    }
}















