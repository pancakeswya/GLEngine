#include "log/log.h"

#ifdef DEBUG

#include <stdlib.h>
#include <stdio.h>

static inline void log(const char* header, const char* log, const char* file, const int line) {
    fprintf(stderr,
          "[%s] :\n"
          "Source file:%s\n"
          "Line:%d\n"
          "Log:%s\n\n", header, file, line, log);
}

void log_info(const char* info, const char* file, const int line) {
    log("Info", info, file, line);
}

void log_gl_error(const GLuint who, const char* file, const int line) {
    GLint length;
    const char header[] = "ErrorGl";
    glGetShaderiv(who, GL_INFO_LOG_LENGTH, &length);
    char* info_buffer = (char*)malloc(length * sizeof(char));
    if (info_buffer == NULL) {
        log(header, "Allocation fail while logging other error, "
                 "check error code", file, line);
        return;
    }
    glGetShaderInfoLog(who, length, &length, info_buffer);
    log(header,info_buffer, file, line);
    free(info_buffer);
}

void log_error(const error err, const char* file, const int line) {
    log("Error", error_str(err), file, line);
}

#endif // DEBUG