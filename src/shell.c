#include "shell.h"

int main(void) {
    struct passwd *pw;
    uid_t uid;
    struct utsname systemInfo;
    char homeDirectory[PATH_MAX], currentDirectory[PATH_MAX], directoryOnPrompt[PATH_MAX];
    if (getcwd(homeDirectory, sizeof(homeDirectory)) == NULL) {
        perror("Could not get the home directory. Error with getcwd()");
        return -1;
    }

    while (1) {
        CLEARSCREEN;
        // buffer = getlogin();
        uid = geteuid();
        pw = getpwuid(uid);
        if (pw == NULL) {
            perror("Could not get the pw pointer");
            return -1;
        }
        if (uname(&systemInfo) != 0) {
            perror("Failed to get the username corresponding to the user ID");
            return -1;
        }
        if (getcwd(currentDirectory, sizeof(currentDirectory)) == NULL) {
            perror("Could not get the present working directory. Error with getcwd()");
            return -1;
        }

        // If cwd is under home
        if (startsWith(currentDirectory, homeDirectory)) {
            directoryOnPrompt[0] = '~';
            unsigned long i = 1, j = strlen(homeDirectory);
            while (j < strlen(currentDirectory)) {
                directoryOnPrompt[i] = currentDirectory[j];
                ++j;
                ++i;
            }
            directoryOnPrompt[j] = '\0';
        } else {
            strcpy(directoryOnPrompt, currentDirectory);
        }

        printf("<%s@%s:%s>", pw->pw_name, systemInfo.sysname, directoryOnPrompt);
        break;
    }

    return 0;
}