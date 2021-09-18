#include "ls.h"

int ls(char *saveptr, const char homeDirectory[]) {
    char whitespace[] = " \t\n\f\r\v";

    char path[PATH_MAX];
    if(getcwd(path, sizeof(path)) == NULL) {
        perror("Error with getcwd()");
        return FATAL_ERROR;
    }

    char *arg = strtok_r(NULL, whitespace, &saveptr);
    bool ls_a = false, ls_l = false;
    while(arg != NULL) {
        if(arg[0] == '-') {
            for(unsigned long i = 1; i < strlen(arg); ++i) {
                if(arg[i] == 'a') ls_a = true;
                else if(arg[i] == 'l') ls_l = true;
                else {
                    printf("ls: invalid option -- '%c'\n", arg[i]);
                    return WARNING_ERROR;
                }
            }
        }
        else {
            strcpy(path, arg);
        }
        arg = strtok_r(NULL, whitespace, &saveptr);
    }

    if(DEBUG) {
        printf("flags: ");
        if(ls_a) printf("a");
        if(ls_l) printf("l");
        printf("\n");
        printf("Path: %s\n", path);
    }

    char absolutePath[PATH_MAX];
    if(path[0] == '~') {
        char destPath[PATH_MAX + 1];
        strcpy(destPath, homeDirectory);
        unsigned long i = strlen(destPath), j = 1;
        while(j < strlen(path)) {
            destPath[i] = path[j];
            ++i; ++j;
        }
        destPath[i] = '\0';
        strcpy(path, destPath);
    }
    if(realpath(path, absolutePath) == NULL) {
        perror("realpath() error");
        return WARNING_ERROR;
    }

    if(DEBUG) {
        printf("Absolute path: %s\n", absolutePath);
    }

    struct dirent *dirEntry;
    DIR *dir = opendir(absolutePath);
    if(dir == NULL) {
        printf("ls: cannot access '%s'", arg);
        perror("");
        closedir(dir);
        return WARNING_ERROR;
    }

    /* Total Block Size */
    DIR *dirCopy = opendir(absolutePath);
    if(dirCopy == NULL) {
        printf("ls: cannot access '%s'", arg);
        perror("");
        closedir(dirCopy);
        return WARNING_ERROR;
    }
    long totalBlocks = 0;
    while((dirEntry = readdir(dirCopy)) != NULL) {
        if(!ls_a && strlen(dirEntry->d_name) > 0 && dirEntry->d_name[0] == '.') {
            continue;
        }
        struct stat statbuf;
        char absPathOfFile[PATH_MAX];
        strcpy(absPathOfFile, absolutePath);
        size_t n = strlen(absPathOfFile);
        if(absolutePath[n - 1] != '/') {
            if(n + 1 < PATH_MAX) {
                absPathOfFile[n] = '/';
                absPathOfFile[n + 1] = '\0';
            }
            else {
                printf("Error: path length cannot exceed PATH_MAX\n");
                closedir(dir);
                return WARNING_ERROR;
            }
        }
        if(strlen(absPathOfFile) + strlen(dirEntry->d_name) >= PATH_MAX) {
            printf("Error: path length cannot exceed PATH_MAX\n");
            closedir(dir);
            return WARNING_ERROR;
        }
        strcat(absPathOfFile, dirEntry->d_name);
        if(stat(absPathOfFile, &statbuf) != 0) {
            perror("stat() error");
            closedir(dirCopy);
            return WARNING_ERROR;
        }
        totalBlocks += statbuf.st_blocks;
    }
    printf("total %ld\n", totalBlocks / 2);
    closedir(dirCopy);

    while((dirEntry = readdir(dir)) != NULL) {
        if(!ls_a && strlen(dirEntry->d_name) > 0 && dirEntry->d_name[0] == '.') {
            continue;
        }
        if(!ls_l) {
            printf("%s\n", dirEntry->d_name);
        }
        else {
            struct stat statbuf;
            char absPathOfFile[PATH_MAX];
            strcpy(absPathOfFile, absolutePath);
            size_t n = strlen(absPathOfFile);
            if(absolutePath[n - 1] != '/') {
                if(n + 1 < PATH_MAX) {
                    absPathOfFile[n] = '/';
                    absPathOfFile[n + 1] = '\0';
                }
                else {
                    printf("Error: path length cannot exceed PATH_MAX\n");
                    closedir(dir);
                    return WARNING_ERROR;
                }
            }
            if(strlen(absPathOfFile) + strlen(dirEntry->d_name) >= PATH_MAX) {
                printf("Error: path length cannot exceed PATH_MAX\n");
                closedir(dir);
                return WARNING_ERROR;
            }
            strcat(absPathOfFile, dirEntry->d_name);
            if(stat(absPathOfFile, &statbuf) != 0) {
                perror("stat() error");
                closedir(dir);
                return WARNING_ERROR;
            }

            /* File Type */
            if(statbuf.st_mode & S_IFDIR) printf("d");
            else if(statbuf.st_mode & S_IFREG) printf("-");
            else if(statbuf.st_mode & S_IFBLK) printf("b");
            else if(statbuf.st_mode & S_IFCHR) printf("c");
            else if(statbuf.st_mode & S_IFSOCK) printf("s");
            else if(statbuf.st_mode & S_IFLNK) printf("l");

            /* Owner Permissions */
            if(statbuf.st_mode & S_IRUSR) printf("r");
            else printf("-");
            if(statbuf.st_mode & S_IWUSR) printf("w");
            else printf("-");
            if(statbuf.st_mode & S_IXUSR) printf("x");
            else printf("-");

            /* Group Permissions */
            if(statbuf.st_mode & S_IRGRP) printf("r");
            else printf("-");
            if(statbuf.st_mode & S_IWGRP) printf("w");
            else printf("-");
            if(statbuf.st_mode & S_IXGRP) printf("x");
            else printf("-");

            /* Others Permissions */
            if(statbuf.st_mode & S_IROTH) printf("r");
            else printf("-");
            if(statbuf.st_mode & S_IWOTH) printf("w");
            else printf("-");
            if(statbuf.st_mode & S_IXOTH) printf("x");
            else printf("-");

            /* Number of Hard Links */
            printf(" %3zu ", statbuf.st_nlink);

            /* Owner Name */
            struct passwd *pw = getpwuid(statbuf.st_uid);
            if(pw == NULL) {
                perror("Could no get the pw pointer");
                closedir(dir);
                return FATAL_ERROR;
            }
            printf("%s ", pw->pw_name);

            /* Group Name */
            struct group *grp = getgrgid(statbuf.st_gid);
            if(pw == NULL) {
                perror("Could no get the pw pointer");
                closedir(dir);
                return FATAL_ERROR;
            }
            printf("%s ", grp->gr_name);

            /* File Size in Bytes */
            printf("%10ld ", statbuf.st_size);

            /* Time of Last Modification */
            struct tm *T = localtime(&(statbuf.st_mtim.tv_sec));
            time_t currentTime;
            time(&currentTime);
            time_t timePassed = currentTime - statbuf.st_mtim.tv_sec;
            char formattedTime[500];

            /* 182 * 24 * 3600 seconds = 6 months */
            if(timePassed < 182 * 24 * 3600) {
                if(strftime(formattedTime, 500, "%b %d %k:%M", T) == 0) {
                    perror("strftime() error");
                    closedir(dir);
                    return FATAL_ERROR;
                }
            }
            else {
                if(strftime(formattedTime, 500, "%b %d %Y", T) == 0) {
                    perror("strftime() error");
                    closedir(dir);
                    return FATAL_ERROR;
                }
            }
            printf("%s", formattedTime);

            /* Name of the File */
            printf(" %s ", dirEntry->d_name);

            printf("\n");
        }
    }
    closedir(dir);
    return NO_ERROR;
}