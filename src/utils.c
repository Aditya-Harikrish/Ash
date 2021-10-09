#include "utils.h"

bool startsWith(const char *str, const char *substr) {
    return (strncmp(str, substr, strlen(substr)) == 0);
}
void strip(char *str) {
    size_t i, begin = 0, end = strlen(str) - 1;

    while(isspace((unsigned char)str[begin]))
        ++begin;

    while((end >= begin) && isspace((unsigned char)str[end]))
        --end;

    for(i = begin; i <= end; ++i)
        str[i - begin] = str[i];

    str[i - begin] = '\0';
}
long min(long a, long b) {
    return a < b ? a : b;
}
long max(long a, long b) {
    return a > b ? a : b;
}