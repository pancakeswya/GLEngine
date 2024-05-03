#include "base/io.h"

#include <stdio.h>
#include <stdlib.h>

static inline long int file_size(FILE* file) {
    fseek(file, 0L, SEEK_END);
    const long int n_bytes = ftell(file);
    fseek(file, 0L, SEEK_SET);
    if (n_bytes > 0) {
        return n_bytes;
    }
    return 0;
}

error read_file(const char* path, char** content) {
    FILE* file = fopen(path, "rb");
    if(file == NULL) {
        return kErrorNoShaderFileFound;
    }
    const long int n = file_size(file);
    char* buffer = (char*)malloc((n + 1) * sizeof(char));
    if(buffer == NULL) {
        return kErrorAllocationFailed;
    }
    const size_t count = fread(buffer, sizeof(char), n, file);
    if (count != (size_t) n) {
        return kErrorReadingShaderFile;
    }
    buffer[count] = '\0';
    *content = buffer;

    fclose(file);

    return kErrorNil;
}
