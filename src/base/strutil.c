#include "base/strutil.h"

#include <string.h>
#include <stdlib.h>

char* concat (
    const char *first,
    const char *second_p,
    const char *second_e
) {
    const size_t len_first = (first == NULL) ? 0 : strlen(first);
    const size_t len_second = second_e - second_p;
    char *str = (char *)malloc(len_first + len_second + 1);
    if (str == NULL) {
        return NULL;
    }
    if (first) {
        memcpy(str, first, len_first);
    }
    memcpy(str + len_first, second_p, len_second);
    str[len_first + len_second] = '\0';
    return str;
}