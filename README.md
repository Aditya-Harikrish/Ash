# Ash

A shell I made in C.

## Instructions to Run

1. Clone the repository and `cd` into it.
2. Enter `make` in your shell
3. Enter `./ash` to run the shell.

## Organisation

The C files are present in the `src` directory, th header files in the `header` and the object files in `src/obj`.

1. `main.c` contains the driver code.
2. `shell.c` and `shell.h` contain `execute()` which decides which functions to call to execute a command.
3. `ls.c` and `ls.h` contain code for `ls`, `builtin.c`
4. `history.c` and `history.h` add commands to the history.
5. `utils.c` and `utils.h` are for miscellaneous things used in multiple files.
6. `prompt.c` and `prompt.h` contain code for the prompt in the terminal.
7. `header.h` contains the standard header files used throughout the program.
8. `vector.c` and `vector.h` contain code for dynamic arrays of strings, which were not used in the end.

## Assumptions

1. The size of the host's name is at most `HOST_NAME_MAX` (defined in `<limits.h>`) bytes long.
2. The size of a command is at most `MAX_COMMAND_SIZE` bytes long, which is 5000 bytes.
3. For the long output format of `ls`, 6 months equals 182 * 24 * 60 * 60 seconds exactly.
4. The number of space-separated words in a single command is at most `MMAX_NUMBER_OF_ARGS`, which is 100.
