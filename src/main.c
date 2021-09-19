#include "shell.h"
#include "prompt.h"

int main(void) {
    char homeDirectory[PATH_MAX + 1], previousDirectory[PATH_MAX + 1];

    if(getcwd(homeDirectory, sizeof(homeDirectory)) == NULL) {
        perror("Error with getcwd()");
        exit(EXIT_FAILURE);
    }
    strcpy(previousDirectory, homeDirectory);
    CLEARSCREEN;
    int status = NO_ERROR;
    while(status != EXIT_PROGRAM) {
        prompt(homeDirectory);

        char *command = NULL, *saveptr = NULL;
        size_t commandBufferSize = 0;
        ssize_t commandSize = getline(&command, &commandBufferSize, stdin);
        if(commandSize <= 0) {
            perror("getline() error");
            exit(EXIT_FAILURE);
        }
        if(DEBUG) {
            printf("command: %s\n", command);
        }
        char *copyOfCommand = strdup(command);
        addToHistory(copyOfCommand);
        free(copyOfCommand);
        char delim[] = ";\n";
        char *token = strtok_r(command, delim, &saveptr);

        if(token == NULL) continue;
        while(token != NULL) {
            /* Passing token by value to avoid risking modification */
            char *copyOfToken = strdup(token);
            if(copyOfToken == NULL) {
                perror("Insufficient memory");
                exit(EXIT_FAILURE);
            }

            if(DEBUG) {
                printf("Looping through the tokens in the command\n");
                printf("Token: %s\n", copyOfToken);
            }

            status = execute(copyOfToken, homeDirectory, previousDirectory);
            free(copyOfToken);
            if(status == FATAL_ERROR) {
                exit(EXIT_FAILURE);
            }
            else if(status == EXIT_PROGRAM) {
                if(DEBUG) {
                    printf("Exiting the shell.\n");
                }
                break;
            }
            token = strtok_r(NULL, delim, &saveptr);
        }
        free(command);
    }

    return 0;
}