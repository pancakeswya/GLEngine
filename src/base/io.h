#ifndef IO_H_
#define IO_H_

#include "base/error.h"

error read_file(const char* path, char** content);

#endif // IO_H_