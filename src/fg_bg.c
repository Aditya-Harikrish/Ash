#include "fg_bg.h"

void bg_signal_handler(int signal) {
    if(DEBUG) {
        printf("Signal: %d\n", signal);
    }
    int wstatus = 0, child_pid = 0;
    while((child_pid = waitpid(-1, &wstatus, WNOHANG | WUNTRACED)) > 0) {
        // if(child_pid == -1) {
        //     continue;
        // }
        Command *process = NULL;

        if(DEBUG) {
            printf("Searching in bg_processes\n");
        }
        /* if foreground process, continue */
        if((process = C_find(bg_processes, child_pid)) == NULL) {
            if(DEBUG) {
                C_print_vector(bg_processes);
                printf("foreground process ignored in bg_signal_handler()\n");
            }
            continue;
        }
        if(WIFEXITED(wstatus)) {
            // exited normally
            printf("%s with pid %d exited ", process->commandName, process->pid);
            printf("normally\n");
            C_remove(&bg_processes, child_pid);
        }
        else if(WIFSIGNALED(wstatus)) {
            // exited abnormally 
            printf("%s with pid %d exited ", process->commandName, process->pid);
            printf("abnormally\n");
            C_remove(&bg_processes, child_pid);
        }
        if(DEBUG) {
            printf("Looping in bg_signal_handler\n");
        }
    }
}

void add_bg(Command command) {
    command.job_number = job_number;
    ++job_number;
    strip(command.command);
    strip(command.commandName);
    C_pushback(&bg_processes, command);
}

int jobs_sort(const void *a, const void *b) {
    Command *x = (Command *)a;
    Command *y = (Command *)b;
    // char *x_command = strdup(x->command);
    // char *y_command = strdup(y->command);
    // strip(x_command);
    // strip(y_command);
    int ret = strcmp(x->command, y->command);
    // free(y_command);
    // free(x_command);
    return ret;
}

int jobs(char *OGtoken, char *saveptr) {
    char whitespace[] = " \t\n\f\r\v";
    char *copyOfToken = strdup(OGtoken);
    char *arg = strtok_r(NULL, whitespace, &saveptr);
    bool jobs_r = false, jobs_s = false; // flags
    while(arg != NULL) {
        if(arg[0] == '-') {
            for(size_t i = 1; i < strlen(arg); i++) {
                if(arg[i] == 'r') {
                    jobs_r = true;
                }
                else if(arg[i] == 's') {
                    jobs_s = true;
                }
                else {
                    printf("jobs: invalid option -- '%c'\n", arg[i]);
                    free(copyOfToken);
                    return WARNING_ERROR;
                }
            }
        }
        arg = strtok_r(NULL, whitespace, &saveptr);
    }
    if(DEBUG) {
        printf("flags: ");
        if(jobs_r) printf("r");
        if(jobs_s) printf("s");
        printf("\n");
    }
    if(bg_processes.arr == NULL) return NO_ERROR;
    qsort(bg_processes.arr, bg_processes.size, sizeof(Command), jobs_sort);
    for(size_t i = 0; i < bg_processes.size; ++i) {
        char status = get_status(bg_processes.arr[i].pid);
        if((jobs_r && status != 'R') || (jobs_s && status != 'S'))
            continue;

        /* Job number */
        printf("[%d] ", bg_processes.arr[i].job_number);

        /* Job status */
        if(status == 'R') {
            printf("Running ");
        }
        else {
            printf("Stopped ");
        }

        /* The command itself */
        printf("%s ", bg_processes.arr[i].command);

        /* Process ID */
        printf("[%d]\n", bg_processes.arr[i].pid);
    }
    free(copyOfToken);
    return NO_ERROR;
}

int bg(char *saveptr) {
    char whitespace[] = " \t\n\f\r\v";
    char *arg = strtok_r(NULL, whitespace, &saveptr);
    if(arg == NULL) {
        printf("Error: insufficient number of arguments!\n");
        return WARNING_ERROR;
    }
    char *endptr = NULL;
    int job_no = (int)strtol(arg, &endptr, 10);
    if(endptr == arg || errno == EINVAL || errno == ERANGE) {
        printf("Error: %s is not a valid number!\n", arg);
        perror("");
        return WARNING_ERROR;
    }

    /* Find job in bg_processes */
    bool found = false; pid_t job_pid = -1;
    for(size_t i = 0; i < bg_processes.size; ++i) {
        if(bg_processes.arr[i].job_number == job_no) {
            found = true;
            job_pid = bg_processes.arr[i].pid;
            break;
        }
    }
    if(!found) {
        printf("Error: no job found with job number %d\n", job_no);
    }

    /* Send the signals to the job */
    if(kill(job_pid, SIGTTOU) == -1) {
        perror("kill() error");
        return WARNING_ERROR;
    }
    if(kill(job_pid, SIGCONT)) {
        perror("kill() error");
        return WARNING_ERROR;
    }
    return NO_ERROR;
}

int sig(char *saveptr) {
    char whitespace[] = " \t\n\f\r\v";

    /* Get the job number */
    char *arg = strtok_r(NULL, whitespace, &saveptr);
    if(arg == NULL) {
        printf("Error: insufficient n   umber of arguments provided\n");
        return WARNING_ERROR;
    }
    char *endptr = NULL;
    int job_no = (int)strtol(arg, &endptr, 10);
    if(endptr == arg || errno == EINVAL || errno == ERANGE) {
        printf("Error: %s is not a valid number!\n", arg);
        return WARNING_ERROR;
    }

    /* Get the signal number */
    arg = strtok_r(NULL, whitespace, &saveptr);
    if(arg == NULL) {
        printf("Error: insufficient number of arguments provided\n");
        return WARNING_ERROR;
    }
    endptr = NULL;
    int signal_no = (int)strtol(arg, &endptr, 10);
    if(endptr == arg || errno == EINVAL || errno == ERANGE) {
        printf("Error: %s is not a valid number!\n", arg);
        return WARNING_ERROR;
    }

    /* Find the pid corrsponding to job_no */
    pid_t pid = -1;
    for(size_t i = 0; i < bg_processes.size; ++i) {
        if(bg_processes.arr[i].job_number == job_no) {
            pid = bg_processes.arr[i].pid;
            break;
        }
    }
    if(pid == -1) {
        printf("Error: job with job number %d not found!\n", job_no);
        return WARNING_ERROR;
    }

    /* Send signal_no to job_no */
    if(kill(pid, signal_no) == -1) {
        perror("kill() error");
        return WARNING_ERROR;
    }
    return NO_ERROR;
}

int background_command(char *copyOfToken, char *commandName) {
    char whitespace[] = " \t\n\f\r\v";

    if(DEBUG) {
        printf("background process\n");
    }
    pid_t pid = fork();
    after_fork_check(pid);

    /* Child Process */
    if(pid == 0) {
        if(DEBUG) {
            printf("After addbg(): ");
            C_print_vector(bg_processes);
        }

        /* Execute this child */
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
        /* Add this process to the bg_processes */
        Command bg_command;
        strcpy(bg_command.command, copyOfToken);
        strcpy(bg_command.commandName, commandName);
        bg_command.pid = pid;
        add_bg(bg_command);
        // setpgid(pid, 0);
        // tcsetpgrp(0, getpgrp());
        printf("%d\n", pid);;
        free(copyOfToken);
    }
    return NO_ERROR;
}

int foreground_command(char *copyOfToken, char *commandName) {
    char whitespace[] = " \t\n\f\r\v";

    if(DEBUG) {
        printf("foreground process\n");
    }
    pid_t pid = fork();
    after_fork_check(pid);
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
        int exit_status = EXIT_SUCCESS;
        if(execvp(commandName, args) == -1) {
            printf("%s: command not found\n", commandName);
            exit_status = EXIT_FAILURE;
        }
        for(int j = 0; j < i; ++j) {
            free(args[j]);
        }
        exit(exit_status);
    }
    /* Parent Process */
    else {
        pid_foreground = pid;
        int wstatus = 0;
        waitpid(pid, &wstatus, WUNTRACED);
        free(copyOfToken);
        pid_foreground = -1;
    }
    return NO_ERROR;
}

void ctrlC(int s) {
    if(DEBUG) {
        printf("Detected ctrl + C, signal %d\n", s);
    }
    if(pid_foreground != -1) {
        kill(pid_foreground, SIGINT);
    }
}





