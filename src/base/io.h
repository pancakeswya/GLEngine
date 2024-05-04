#ifndef BASE_IO_H_
#define BASE_IO_H_

#include "base/error.h"

#include <stddef.h>

error read_file(const char* path, char** content, size_t* count);

#endif // BASE_IO_H_