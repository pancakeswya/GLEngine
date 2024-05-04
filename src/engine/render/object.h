#ifndef ENGINE_RENDER_OBJECT_H_
#define ENGINE_RENDER_OBJECT_H_

#include "base/error.h"
#include "base/vector.h"
#include "obj/data.h"

typedef struct RenderObject {
    vector* indices;
    vector* vertices;
    vector* usemtl;
    vector* mtl;
} RenderObject;

error render_object_create(RenderObject* object, ObjData* data);
void render_object_free(const RenderObject* object);

#endif // ENGINE_RENDER_OBJECT_H_