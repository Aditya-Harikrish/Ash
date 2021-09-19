#include "utils.h"

bool startsWith(const char *str, const char *substr) {
    return (strncmp(str, substr, strlen(substr)) == 0);
}
long min(long a, long b) {
    return a < b ? a : b;
}
long max(long a, long b) {
    return a > b ? a : b;
}