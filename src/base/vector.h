#ifndef BASE_VECTOR_H_
#define BASE_VECTOR_H_

#include <stddef.h>

typedef struct vector {
    size_t size;
    size_t cap;
    size_t data_size;
    void* data;
} vector;

vector* vector_create(size_t data_size, size_t init_cap);
void* vector_at(const vector* vec, size_t idx);
void* vector_push(vector* vec, size_t count);
void vector_free(vector* vec);

#endif // BASE_VECTOR_H_