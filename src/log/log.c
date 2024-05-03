#include "log/log.h"

#include <stdlib.h>
#include <stdio.h>

static inline void log_info(const char* info, const char* file, const int line) {
    fprintf(stderr,
      "Error\n"
      "Source file:%s\n"
      "Line:%d\n"
      "Log:%s", file, line, info);
}

void log_gl_error(const GLuint who, const char* file, const int line) {
    GLint length;
    glGetShaderiv(who, GL_INFO_LOG_LENGTH, &length);
    char* info_buffer = (char*)malloc(length * sizeof(char));
    if (info_buffer == NULL) {
        log_info("Allocation fail while logging other error, "
                 "check error code", file, line);
        return;
    }
    glGetShaderInfoLog(who, length, &length, info_buffer);
    log_info(info_buffer, file, line);
    free(info_buffer);
}

void log_error(const error err, const char* file, const int line) {
    log_info(error_str(err), file, line);
}