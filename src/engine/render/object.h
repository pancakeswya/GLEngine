#ifndef ENGINE_RENDER_OBJECT_H_
#define ENGINE_RENDER_OBJECT_H_

#include "base/error.h"
#include "base/vector.h"
#include "obj/data.h"


typedef struct RenderMap {
    unsigned char* image;
    int width, height, n_channels;
} RenderMap;

typedef struct RenderMaps {
    RenderMap kd, ns, bump;
} RenderMaps;

typedef struct RenderObject {
    vector* indices;
    vector* vertices;
    vector* maps;
    vector* usemtl;
} RenderObject;

error render_object_create(RenderObject* object, ObjData* data);
void render_object_free(const RenderObject* object);

#endif // ENGINE_RENDER_OBJECT_H_