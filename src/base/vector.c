#include "base/vector.h"

#include <stdlib.h>

#include <stdio.h>

vector* vector_create(const size_t data_size, const size_t init_cap) {
    vector* vec = (vector*)malloc(sizeof *vec);
    if (vec == NULL) {
        return NULL;
    }
    if (init_cap == 0) {
        *vec = (vector){ .data_size = data_size };
    } else {
        void* ptr = malloc(init_cap * data_size);
        if (ptr == NULL) {
            free(vec);
            return NULL;
        }
        *vec = (vector){
            .cap = init_cap,
            .data_size = data_size,
            .data = ptr
        };
    }
    return vec;
}

void* vector_push(vector* vec, const size_t count) {
    if (vec->size + count >= vec->cap) {
        const size_t cap = vec->cap * 2 + count;
        void* ptr = realloc(vec->data, cap * vec->data_size);
        if (ptr == NULL) {
            return NULL;
        }
        vec->data = ptr;
        vec->cap = cap;
    }
    const size_t old_size = vec->size;
    vec->size += count;
    return (char*)vec->data + vec->data_size * old_size;
}

void* vector_at(const vector* vec, const size_t idx) {
    if (idx >= vec->size) {
        return NULL;
    }
    return (char*)vec->data + vec->data_size * idx;
}

void vector_free(vector* vec) {
    if (vec == NULL) {
      return;
    }
    free(vec->data);
    free(vec);
}