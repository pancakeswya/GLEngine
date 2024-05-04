#ifndef LOG_LOG_H_
#define LOG_LOG_H_

#include "base/error.h"

#include <GL/glew.h>

void log_gl_error(GLuint who, const char* file, int line);
void log_error(error err, const char* file, int line);

#ifdef DEBUG

#define LOG_GL(who) log_gl_error(who, __FILE__, __LINE__)
#define LOG_ERR(err) log_error(err, __FILE__, __LINE__)

#else

#define LOG_GL(who)
#define LOG_ERR(err)

#endif // DEBUG

#endif // LOG_LOG_H_