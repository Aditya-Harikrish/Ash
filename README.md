# Ash

A shell I made in C.

## Assumptions

1. The size of the host's name is at most `HOST_NAME_MAX` (defined in `<limits.h>`) bytes long.
2. The size of a command is at most `MAX_COMMAND_SIZE` bytes long, which is 2048 bytes.
3. For the long output format of `ls`, 6 months equals 182 * 24 * 60 * 60 seconds exactly.
