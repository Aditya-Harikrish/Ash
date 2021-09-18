#include "utils.h"

bool startsWith(const char *str, const char *substr) {
    return (strncmp(str, substr, strlen(substr)) == 0);
}
