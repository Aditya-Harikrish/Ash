#include "history.h"
#include "shell.h"

void addToHistory(char *command) {
    int linecount = 0;
    char historyFileName[] = "history.log";
    FILE *fp = fopen("history.log", "a");
    if(fp == NULL) {
        perror("fopen error");
        return;
    }
    fclose(fp);

    fp = fopen("history.log", "r");
    char ch;
    while((ch = (char)getc(fp)) != EOF) {
        if(ch == '\n') {
            ++linecount;
        }
    }
    fclose(fp);

    if(DEBUG) {
        printf("Number of lines in history.log = %d\n", linecount);
    }

    if(linecount >= 20) {
        char lastCommand[MAX_COMMAND_SIZE];
        FILE *fp = fopen("history.log", "r");
        char ch = 'a'; // dummy value
        int lineNumber = 1;
        while(ch != EOF && (ch = (char)getc(fp)) != EOF) {
            if(ch == '\n') {
                ++lineNumber;
            }
            if(lineNumber == 20) {
                int i = 0;
                while((ch = (char)getc(fp)) != EOF && i < (int)sizeof(lastCommand)) {
                    lastCommand[i] = ch;
                    ++i;
                }
                if(DEBUG) {
                    printf("last command = %s\n", lastCommand);
                }
                if(!strcmp(command, lastCommand)) {
                    fclose(fp);
                    return;
                }
            }
        }
        fclose(fp);

        /* Remove first line */
        FILE *fp1 = NULL, *fp2 = NULL;

        lineNumber = 1;

        fp1 = fopen(historyFileName, "r");
        fp2 = fopen("copy.log", "w");
        rewind(fp1);
        while((ch = (char)getc(fp1)) != EOF) {
            if(lineNumber != 1) {
                fprintf(fp2, "%c", ch);
            }
            if(ch == '\n') {
                lineNumber++;
            }
        }
        remove(historyFileName);
        rename("copy.log", historyFileName);
        fclose(fp1);
        fclose(fp2);
    }
    fp = fopen(historyFileName, "a");
    fprintf(fp, "%s", command);
    fclose(fp);
}

int history(char *saveptr) {
    char whitespace[] = " \t\n\f\r\v";
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
            printf("Error: %s is not a valid number!\n", arg);
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

    long startFrom = max(1, linecount - count + 1);
    for(long i = 1; i <= linecount && (readSize = getline(&line, &bufSize, fp)) != -1; ++i) {
        if(i >= startFrom)
            printf("%s", line);
    }

    fclose(fp);
    free(line);
    return NO_ERROR;
}



