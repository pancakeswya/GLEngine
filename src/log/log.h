#ifndef LOG_H_
#define LOG_H_

#include "base/error.h"

#include <GL/glew.h>

void log_gl_error(GLuint who, const char* file, int line);
void log_error(error err, const char* file, int line);

#define LOG_GL(who) log_gl_error(who, __FILE__, __LINE__)
#define LOG_ERR(err) log_error(err, __FILE__, __LINE__)

#endif // LOG_H_