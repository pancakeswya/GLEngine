#ifndef BASE_STRUTIL_H_
#define BASE_STRUTIL_H_

#include <stdbool.h>

static inline bool is_space(const char c) {
    return (c == ' ') || (c == '\t') || (c == '\r');
}

static inline bool is_digit(const char c) { return (c >= '0') && (c <= '9'); }

static inline bool is_end_of_name(const char c) {
    return (c == '\t') || (c == '\r') || (c == '\n');
}

static inline const char *skip_space(const char *ptr) {
    for (; is_space(*ptr); ++ptr)
        ;
    return ptr;
}

static inline const char *skip_line(const char *ptr) {
    for (; *ptr != '\n'; ++ptr)
        ;
    return ++ptr;
}

static inline const char *end_of_name(const char *ptr) {
    for (; !is_end_of_name(*ptr); ++ptr)
        ;
    return ptr;
}

char* concat(
    const char *first,
    const char *second_p,
    const char *second_e
);

#endif // BASE_STRUTIL_H_