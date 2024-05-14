#ifndef ENGINE_RENDER_OBJECT_H_
#define ENGINE_RENDER_OBJECT_H_

#include "base/error.h"
#include "base/vector.h"

typedef struct RenderObject {
    vector* indices;
    vector* vertices;
    vector* maps;
    vector* usemtl;
} RenderObject;

error render_object_create(const char* path, RenderObject* object);
void render_object_free(const RenderObject* object);

#endif // ENGINE_RENDER_OBJECT_H_