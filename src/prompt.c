#include "prompt.h"
void prompt(char homeDirectory[]) {
    struct passwd *pw;
    uid_t uid;
    char hostName[HOST_NAME_MAX + 1], currentDirectory[PATH_MAX + 1], directoryOnPrompt[PATH_MAX + 1];
    uid = geteuid();
    pw = getpwuid(uid);
    if(pw == NULL) {
        perror("Could not get the pw pointer");
        exit(EXIT_FAILURE);
    }

    if(gethostname(hostName, HOST_NAME_MAX + 1) != 0) {
        perror("gethostname() error");
        exit(EXIT_FAILURE);
    }

    if(getcwd(currentDirectory, sizeof(currentDirectory)) == NULL) {
        perror("Could not get the present working directory. Error with getcwd()");
        exit(EXIT_FAILURE);
    }

    // If cwd is under home
    if(startsWith(currentDirectory, homeDirectory)) {
        directoryOnPrompt[0] = '~';
        directoryOnPrompt[1] = '\0';
        unsigned long i = 1, j = strlen(homeDirectory);
        while(j < strlen(currentDirectory)) {
            directoryOnPrompt[i] = currentDirectory[j];
            ++j; ++i;
        }
        directoryOnPrompt[i] = '\0';
    }
    else {
        strcpy(directoryOnPrompt, currentDirectory);
    }

    /* Uncoloured Output */
    // printf("<%s@%s:%s> ", pw->pw_name, hostName, directoryOnPrompt);

    /* Coloured Output */
    printf(BRIGHT_GREEN "<%s@%s" RESET, pw->pw_name, hostName);
    printf(":" BRIGHT_BLUE "%s" BRIGHT_GREEN ">" RESET, directoryOnPrompt);
}