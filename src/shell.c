#include "shell.h"

void setup() {
    pid_foreground = -1;

    /* ctrl + C */
    // signal(SIGINT, SIG_IGN);
    struct sigaction ctrC_handler;

    ctrC_handler.sa_handler = ctrlC;
    sigemptyset(&ctrC_handler.sa_mask);
    ctrC_handler.sa_flags = 0;

    sigaction(SIGINT, &ctrC_handler, NULL);

    /* ctrl + Z */
    struct sigaction ctrZ_handler;

    ctrZ_handler.sa_handler = ctrlZ;
    sigemptyset(&ctrZ_handler.sa_mask);
    ctrZ_handler.sa_flags = 0;

    sigaction(SIGINT, &ctrZ_handler, NULL);
}

int execute(char *OGtoken, char homeDirectory[], char previousDirectory[]) {
    strip(OGtoken);
    char whitespace[] = " \t\n\f\r\v";

    if(isPipe(OGtoken)) {
        int numberOfPipes = 0;
        for(size_t i = 0; i < strlen(OGtoken); ++i) {
            if(OGtoken[i] == '|') ++numberOfPipes;
        }
        if(DEBUG) {
            printf("Number of pipes = %d\n", numberOfPipes);
        }
        char *saveptr_pipe = NULL;
        char *command = strtok_r(OGtoken, "|", &saveptr_pipe);
        int commandNum = 0;
        int pipe1[2], pipe2[2];
        while(command != NULL) {
            char commandToExecute[MAX_COMMAND_SIZE];
            strcpy(commandToExecute, command);
            if(DEBUG) {
                printf("commandToExecute: %s\n", commandToExecute);
            }
            if(commandNum % 2 != 0) {
                int z = pipe(pipe2);
                if(z < 0)
                    perror("Error: pipe could not be created\n");
            }
            else {
                int z = pipe(pipe1);
                if(z < 0)
                    perror("Error: pipe could not be created\n");
            }

            pid_t pid = fork();
            after_fork_check(pid);
            /* Child Process */
            if(pid == 0) {
                if(commandNum == 0) {
                    dup2(pipe1[1], STDOUT_FILENO);
                    close(pipe1[0]);
                    // strcpy(temp, commands[commandNum]);
                }
                else if(commandNum == numberOfPipes - 1) {
                    if(commandNum % 2 == 0) {
                        dup2(pipe2[0], STDIN_FILENO);
                    }
                    else {
                        dup2(pipe1[0], STDIN_FILENO);
                    }

                }

                else if(commandNum % 2 == 0) {
                    dup2(pipe2[0], STDIN_FILENO);
                    close(pipe1[0]);
                    dup2(pipe1[1], STDOUT_FILENO);
                }

                else if(commandNum % 2 == 1) {
                    close(pipe1[1]);
                    dup2(pipe1[0], STDIN_FILENO);
                    dup2(pipe2[1], STDOUT_FILENO);
                }

                // strcpy(temp, commands[commandNum]);

                // r = redirection_check(temp);
                // if(r == 1 || r == 2)
                //     redirection(part, ii, commands[commandNum], r);

                // else {
                //     int z = execvp(part[0], part);
                //     if(z < 0)
                //         perror("Error: command not found\n");

                //     if(r == 0)
                //         exit(0);
                // }
                execute(command, homeDirectory, previousDirectory);
                // exit(EXIT_SUCCESS);
            }
            /* Parent process */
            else {
                wait(NULL);

                if(commandNum == 0) {
                    close(pipe1[1]);
                    // if(inp == 1) {
                    //     dup2(oldin, STDIN_FILENO);
                    //     // close(ifd);
                    // }
                }
                else if(commandNum == numberOfPipes - 1) {
                    if(commandNum % 2 == 0) {
                        close(pipe2[0]);
                    }
                    else {
                        close(pipe1[0]);
                    }

                    // if(outp == 1) {
                    //     dup2(oldout, STDOUT_FILENO);
                    //     // close(ofd);
                    // }
                }
                else if(commandNum % 2 == 0) {
                    close(pipe1[1]);
                    close(pipe2[0]);
                }
                else if(commandNum % 2 == 1) {
                    close(pipe2[1]);
                    close(pipe1[0]);
                }
            }

            int status = execute(commandToExecute, homeDirectory, previousDirectory);
            ++commandNum;
            if(status != NO_ERROR) {
                return status;
            }
            command = strtok_r(NULL, "|", &saveptr_pipe);
        }
        return NO_ERROR;
    }

    char token[MAX_COMMAND_SIZE];
    strcpy(token, OGtoken);
    if(DEBUG) {
        printf("Executing the command: %s\n", token);
    }

    char outputFilePath[PATH_MAX], inputFilePath[PATH_MAX];
    outputFilePath[0] = '\0';
    inputFilePath[0] = '\0';
    int outputMode = -1;
    int check = removeAndSetIO(token, outputFilePath, inputFilePath, &outputMode);
    outputFilePath[0] = '\0';
    inputFilePath[0] = '\0';
    outputMode = -1;
    if(DEBUG) {
        fprintf(stderr, "token after removeAndSetIO(): %s\n", token);
    }
    if(check != NO_ERROR) {
        if(check == WARNING_ERROR) {
            fprintf(stderr, "removeAndSetIO() returned a warning error\n");
        }
        return check;
    }

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
    else if(!strcmp(commandName, "fg")) {
        return fg(saveptr);
    }
    else if(!strcmp(commandName, "bg")) {
        return bg(saveptr);
    }
    else if(!strcmp(commandName, "sig")) {
        return sig(saveptr);
    }
    else if(!strcmp(commandName, "replay")) {
        int interval = 1, period = 1;
        char *arg = strtok_r(NULL, whitespace, &saveptr);
        char commandToExecute[MAX_COMMAND_SIZE];
        commandToExecute[0] = '\0';
        if(arg == NULL) {
            fprintf(stderr, "Error: arguments missing\n");
            return WARNING_ERROR;
        }
        while(arg != NULL) {
            if(!strcmp(arg, "-command")) {
                arg = strtok_r(NULL, whitespace, &saveptr);
                while(arg != NULL && strcmp(arg, "-interval") && strcmp(arg, "period")) {
                    strcat(commandToExecute, arg);
                    strcat(commandToExecute, " ");
                    arg = strtok_r(NULL, whitespace, &saveptr);
                }
            }
            if(arg == NULL) break;
            if(!strcmp(arg, "-interval")) {
                arg = strtok_r(NULL, whitespace, &saveptr);
                if(arg == NULL) {
                    fprintf(stderr, "Error: arguments missing\n");
                    return WARNING_ERROR;
                }
                char *endptr = NULL;
                interval = (int)strtol(arg, &endptr, 10);
                if(endptr == arg || errno == EINVAL || errno == ERANGE) {
                    printf("Error: %s is not a valid number!\n", arg);
                    perror("");
                    return WARNING_ERROR;
                }
                arg = strtok_r(NULL, whitespace, &saveptr);
            }
            if(arg == NULL) break;
            if(!strcmp(arg, "-period")) {
                arg = strtok_r(NULL, whitespace, &saveptr);
                if(arg == NULL) {
                    fprintf(stderr, "Error: arguments missing\n");
                    return WARNING_ERROR;
                }
                char *endptr = NULL;
                period = (int)strtol(arg, &endptr, 10);
                if(endptr == arg || errno == EINVAL || errno == ERANGE) {
                    printf("Error: %s is not a valid number!\n", arg);
                    perror("");
                    return WARNING_ERROR;
                }
                arg = strtok_r(NULL, whitespace, &saveptr);
            }
        }
        if(DEBUG) {
            fprintf(stderr, "Command in replay: %s\n", commandToExecute);
            fprintf(stderr, "Period in replay: %d\n", period);
            fprintf(stderr, "Interval in replay: %d\n", interval);
        }
        int timePassed = 0;
        while(timePassed <= period) {
            sleep((unsigned)interval);
            timePassed += interval;
            execute(commandToExecute, homeDirectory, previousDirectory);
        }
        return NO_ERROR;
    }

    /* All other commands */
    else {
        if(DEBUG) {
            printf("Going to use execvp()\n");
        }
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















