# Ash

A shell I made in C.

## Prerequisites

You need to have `clang` installed and in your path. Ash has been developed using `clang-12`.

## Instructions to Run

1. Clone the repository and `cd` into it.
2. Enter `make` in your shell
3. Enter `./ash` to run the shell.
4. [Optional] To remove intermediate files, run `make clean`.

## Organisation

The C files are present in the `src` directory, th header files in the `header` and the object files in `src/obj`.

1. `main.c` contains the driver code.
2. `shell.c` and `shell.h` contain `execute()` which decides which functions to call to execute a command.
3. `ls.c` and `ls.h` contain code for `ls`.
4. `builtin.c` and `builtin.h` contain code for the smaller commands - `echo`, `cd`, `pwd` and `pinfo`.
5. `history.c` and `history.h` contain code for history-related tasks.
6. `utils.c` and `utils.h` are for miscellaneous things used in multiple files.
7. `prompt.c` and `prompt.h` contain code for the prompt in the terminal.
8. `header.h` contains the standard header files used throughout the program.
9. `vector.c` and `vector.h` contain code for dynamic arrays of strings.
10. `fg_bg.c` and `fg_bg.h` contain code for signal handling, including management of foreground and background processes.
11. `io.c` and `io.h` contain code for I/O redirection and pipelining.

## Assumptions

1. The size of the host's name is at most `HOST_NAME_MAX` (defined in `<limits.h>`) bytes long.
2. The size of a command is at most `MAX_COMMAND_SIZE` bytes long, which is 5000 bytes.
3. For the long output format of `ls`, 6 months equals 182 * 24 * 60 * 60 seconds exactly.
4. The number of space-separated words in a single command is at most `MAX_NUMBER_OF_ARGS`, which is 100.
