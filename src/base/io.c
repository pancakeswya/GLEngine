#include "base/io.h"
#include "log/log.h"

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

error read_file(const char* path, char** content, size_t* count) {
    FILE* file = fopen(path, "rb");
    if(file == NULL) {
        LOG_ERR(kErrorNoFileFound);
        return kErrorNoFileFound;
    }
    const long int n = file_size(file);
    char* buffer = (char*)malloc((n + 1) * sizeof(char));
    if(buffer == NULL) {
        LOG_ERR(kErrorAllocationFailed);
        return kErrorAllocationFailed;
    }
    const size_t read = fread(buffer, sizeof(char), n, file);
    if (read != (size_t) n) {
        LOG_ERR(kErrorReadingFile);
        return kErrorReadingFile;
    }
    buffer[read] = '\0';
    if (count != NULL) {
        *count = read;
    }
    *content = buffer;

    fclose(file);

    return kErrorNil;
}
