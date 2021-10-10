#include "header.h"
#include "vector.h"
#include "utils.h"
#include "builtin.h"
#ifndef FG_BG_H
#define FG_BG_H

/* Vector of strings to store background processes */
extern CVector bg_processes;

/* Job number assigned sequentially to background processes */
extern int job_number;

void bg_signal_handler(int signal);
void add_bg(Command command); // Adds command to bg_processes

/* jobs command */
int jobs(char *OGtoken, char *saveptr);
int jobs_sort(const void *a, const void *b);

/* bg command */
int bg(char *saveptr);

/* fg command */
int fg(char *saveptr);

/* sig command */
int sig(char *saveptr);

/* background and foreground processes using execvp */
int background_command(char *copyOfToken, char *commandName);
int foreground_command(char *copyOfToken, char *commandName);

void ctrlC(int s);
void ctrlZ(int s);

#endif