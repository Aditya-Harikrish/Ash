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

char get_status(pid_t pid) {
    char statPath[PATH_MAX];
    sprintf(statPath, "/proc/%d/stat", pid);
    if(DEBUG) {
        printf("stat path: %s\n", statPath);
    }

    FILE *statFile;
    if((statFile = fopen(statPath, "r")) == NULL) {
        printf("Could not open %s\n", statPath);
        perror("");
        return WARNING_ERROR;
    }
    char status, fileName[FILENAME_MAX];
    pid_t pid_unused = -1;
    fscanf(statFile, "%d %s %c", &pid_unused, fileName, &status);
    return status;
}

int pinfo(char *homeDirectory, char *saveptr) {
    char whitespace[] = " \t\n\f\r\v";
    char *arg = strtok_r(NULL, whitespace, &saveptr);
    pid_t pid = getpid();
    char statPath[PATH_MAX], cmdlinePath[PATH_MAX], exePath[PATH_MAX];
    if(arg != NULL) {
        char *endptr = NULL;
        pid = (pid_t)strtol(arg, &endptr, 10);
        if(endptr == arg || errno == EINVAL || errno == ERANGE) {
            printf("Error: %s is not a valid process ID!", arg);
            perror("");
            return WARNING_ERROR;
        }
    }
    sprintf(statPath, "/proc/%d/stat", pid);
    sprintf(cmdlinePath, "/proc/%d/cmdline", pid);
    sprintf(exePath, "/proc/%d/exe", pid);
    if(DEBUG) {
        printf("stat path: %s\n", statPath);
        printf("cmdline path: %s\n", cmdlinePath);
        printf("exe path: %s\n", exePath);
    }

    /* Process ID */
    printf("pid -- %d\n", pid);

    /* Process Status */
    FILE *statFile;
    if((statFile = fopen(statPath, "r")) == NULL) {
        printf("Could not open %s\n", statPath);
        perror("");
        return WARNING_ERROR;
    }
    char status, fileName[FILENAME_MAX];
    pid_t pid_unused = pid;
    fscanf(statFile, "%d %s %c", &pid_unused, fileName, &status);
    if(DEBUG) {
        printf("fileName = %s\n", fileName);
    }
    if(pid_unused != pid) {
        printf("pid not matching pid in  %s\n", statPath);
        fclose(statFile);
        return FATAL_ERROR;
    }
    printf("Process Status -- %c", status);

    // If in foreground 
    if((C_find(bg_processes, pid) == NULL) && (status == 'R' || status == 'S')) {
        printf("+");
    }

    printf("\n");

    /* Memory */
    char statmPath[PATH_MAX];
    sprintf(statmPath, "/proc/%d/statm", pid);
    if(DEBUG) {
        printf("statm path: %s\n", statmPath);
    }
    ssize_t vmsize = 0;
    FILE *statmFile = fopen(statmPath, "r");
    fscanf(statmFile, "%ld", &vmsize);
    printf("memory -- %ld\n", vmsize);

    /* Executable Path */
    char executableRelPath[PATH_MAX];
    char executableAbsPath[PATH_MAX + 1];
    ssize_t bufSize = 0;
    if((bufSize = readlink(exePath, executableAbsPath, sizeof(executableAbsPath) - 1)) == -1) {
        perror("readlink error");
        fclose(statFile);
        return FATAL_ERROR;
    }
    executableAbsPath[bufSize] = '\0';
    char currentDirectory[PATH_MAX + 1];
    if(getcwd(currentDirectory, sizeof(currentDirectory)) == NULL) {
        perror("Error with getcwd()");
        fclose(statFile);
        return FATAL_ERROR;
    }
    if(startsWith(executableAbsPath, currentDirectory)) {
        executableRelPath[0] = '.';
        unsigned long i = 1, j = strlen(currentDirectory);
        while(j < strlen(executableAbsPath)) {
            executableRelPath[i] = executableAbsPath[j];
            ++i; ++j;
        }
        executableRelPath[i] = '\0';
    }
    else if(startsWith(executableAbsPath, homeDirectory)) {
        executableRelPath[0] = '.';
        unsigned long i = 1, j = strlen(homeDirectory);
        while(j < strlen(executableAbsPath)) {
            executableRelPath[i] = executableAbsPath[j];
            ++i; ++j;
        }
        executableRelPath[i] = '\0';
    }
    else {
        strcpy(executableRelPath, executableAbsPath);
        executableRelPath[strlen(executableRelPath)] = '\0';
    }

    printf("Executable Path -- ");
    puts(executableRelPath);
    if(DEBUG) {
        printf("Absolute executable Path -- ");
        puts(executableAbsPath);
    }
    fclose(statmFile);
    fclose(statFile);
    return NO_ERROR;
}




