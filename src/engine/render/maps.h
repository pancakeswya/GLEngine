#ifndef ENGINE_RENDER_MAPS_H_
#define ENGINE_RENDER_MAPS_H_

#include "base/error.h"
#include "base/vector.h"

typedef struct RenderMap {
    unsigned char* image;
    int width, height, n_channels;
} RenderMap;

typedef struct RenderMaps {
    RenderMap kd, ns, bump;
} RenderMaps;

error maps_create(const char* dir_path, const vector* mtl, vector** maps_ptr);
void map_free(const RenderMaps* maps);

#endif // ENGINE_RENDER_MAPS_H_