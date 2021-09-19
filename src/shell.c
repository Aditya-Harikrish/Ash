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
    else if(!strcmp(commandName, "pinfo")) {
        return pinfo(homeDirectory, saveptr);
    }
    else if(!strcmp(commandName, "history")) {
        char *line = NULL;
        size_t bufSize = 0;
        ssize_t readSize;
        FILE *fp = fopen("history.log", "a");
        if(fp == NULL) {
            perror("fopen error");
            return FATAL_ERROR;
        }
        fclose(fp);

        fp = fopen("history.log", "r+");
        char *arg = strtok_r(NULL, whitespace, &saveptr);
        long count = 20;
        char *endptr = NULL;
        if(arg != NULL) {
            long num = strtol(arg, &endptr, 10);
            if(endptr == arg || errno == EINVAL || errno == ERANGE) {
                printf("Error: %s is not a valid number!", arg);
                perror("");
                return WARNING_ERROR;
            }
            count = num;
        }

        long linecount = 0;
        FILE *f = fopen("history.log", "r");
        char ch;
        while((ch = (char)getc(f)) != EOF) {
            if(ch == '\n') {
                ++linecount;
            }
        }
        fclose(f);

        long startFrom = max(0, linecount - count);
        for(long i = 1; i <= linecount && (readSize = getline(&line, &bufSize, fp)) != -1; ++i) {
            if(i >= startFrom)
                printf("%s", line);
        }

        fclose(fp);
        free(line);
        return NO_ERROR;
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
            printf("Error: %s is not a valid number!", arg);
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
    else {
        // printf("%s: command not found\n", commandName);
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
            pid_t pid = fork();

            /* Child Process */
            if(pid == 0) {
                setpgid(0, 0);
                char *args[MAX_NUMBER_OF_ARGS + 1];
                char *saveptr2 = NULL;
                char *arg = strtok_r(copyOfToken, whitespace, &saveptr2);
                int i;
                for(i = 0; i < MAX_NUMBER_OF_ARGS && arg != NULL; ++i) {
                    if(!strcmp(arg, "&")) {
                        --i;
                    }
                    else {
                        args[i] = strdup(arg);
                    }
                    arg = strtok_r(NULL, whitespace, &saveptr2);
                }
                args[i] = NULL;
                execvp(commandName, args);
                for(int j = 0; j < i; ++j) {
                    free(args[j]);
                }
            }
            /* Parent Process */
            else {
                printf("%d\n", pid);
                int wstatus = 0;
                waitpid(pid, &wstatus, WNOHANG | WUNTRACED);
                wait(NULL); // same as waitpid(-1, NULL, 0);
                kill(pid, SIGTERM);
            }
            return NO_ERROR;
        }
        /* Foreground */
        else {
            if(DEBUG) {
                printf("foreground process\n");
            }
            pid_t pid = fork();
            if(pid == 0) {
                setpgid(0, 0);
                char *args[MAX_NUMBER_OF_ARGS + 1];
                char *saveptr2 = NULL;
                char *arg = strtok_r(copyOfToken, whitespace, &saveptr2);
                int i;
                for(i = 0; i < MAX_NUMBER_OF_ARGS && arg != NULL; ++i) {
                    if(!strcmp(arg, "&")) {
                        --i;
                    }
                    else {
                        args[i] = strdup(arg);
                    }
                    arg = strtok_r(NULL, whitespace, &saveptr2);
                }
                args[i] = NULL;
                if(execvp(commandName, args) == -1) {
                    printf("%s: command not found\n", commandName);
                    goto freeMem;
                }
            freeMem:
                for(int j = 0; j < i; ++j) {
                    free(args[j]);
                }
            }
            /* Parent Process */
            else {
                // int wstatus = 0;
                // waitpid(pid, &wstatus, 0);
                wait(NULL); // same as waitpid(-1, NULL, 0);
                kill(pid, SIGTERM);
            }
            return NO_ERROR;
        }
        free(copyOfToken);
        return NO_ERROR;
    }
}