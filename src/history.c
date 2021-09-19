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
        // fseek(fp1, 0, SEEK_SET);
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
        if(DEBUG) {
            printf("\ncopy.log : \n");
            fflush(stdout);
            printf("-------\n");
            // sleep(1);
            char temp;
            FILE *copy = fopen("copy.log", "r");
            while((temp = (char)getc(copy)) != EOF) {
                printf("%c", temp);
            }
            printf("-------\n");
            fclose(copy);
        }
        rename("copy.log", historyFileName);
        fclose(fp1);
        fclose(fp2);
    }
    fp = fopen(historyFileName, "a");
    fprintf(fp, "%s", command);
    fclose(fp);
}