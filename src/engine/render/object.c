#include "engine/render/object.h"
#include "base/map.h"
#include "base/strutil.h"
#include "log/log.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include <string.h>

static error load_map(const char* dir_path, const char* map_path, RenderMap* map) {
    if (map_path == NULL) {
        return kErrorNil;
    }
    char* path = concat(dir_path, map_path, map_path + strlen(map_path));
    if (path == NULL) {
        LOG_ERR(kErrorAllocationFailed);
        return kErrorAllocationFailed;
    }
    int width, height, n_channels;
    unsigned char* image = stbi_load(path, &width, &height, &n_channels, 0);
    free(path);
    if (image == NULL) {
        LOG_ERR(kErrorLoadingMap);
        return kErrorLoadingMap;
    }
    *map = (RenderMap) {
                   .image = image,
                   .width = width,
                   .height = height,
                   .n_channels = n_channels
    };
    return kErrorNil;
}

static void map_free(const RenderMaps* maps) {
    stbi_image_free(maps->kd.image);
    stbi_image_free(maps->ns.image);
    stbi_image_free(maps->bump.image);
}

static error maps_create(const char* dir_path, const vector* mtl, vector** maps_ptr) {
    vector* maps = vector_create(sizeof(RenderMaps), mtl->size);
    for (size_t i = 0; i < mtl->size; ++i) {
        const ObjNewMtl* mtl_ptr = vector_at(mtl, i);
        RenderMaps r_maps;
        error err = load_map(dir_path, mtl_ptr->map_kd, &r_maps.kd);
        if (err != kErrorNil) {
            LOG_ERR(err);
            return err;
        }
        err = load_map(dir_path, mtl_ptr->map_Ns, &r_maps.ns);
        if (err != kErrorNil) {
            map_free(&r_maps);
            LOG_ERR(err);
            return err;
        }
        err = load_map(dir_path, mtl_ptr->map_bump, &r_maps.bump);
        if (err != kErrorNil) {
            map_free(&r_maps);
            LOG_ERR(err);
            return err;
        }
        RenderMaps* r_maps_ptr = vector_push(maps, 1);
        if (r_maps_ptr == NULL) {
            map_free(&r_maps);
            LOG_ERR(kErrorAllocationFailed);
            return kErrorAllocationFailed;
        }
        *r_maps_ptr = r_maps;
    }
    *maps_ptr = maps;
    return kErrorNil;
}

static error optimize_geometry(const RenderObject* object, const ObjData* data) {
    hashmap* indices_map = hashmap_create();
    if (indices_map == NULL) {
        LOG_ERR(kErrorAllocationFailed);
        return kErrorAllocationFailed;
    }
    ObjIndex next_combined_idx = 0;
    for(size_t i = 0; i < data->indices->size; ++i) {
        const ObjIndices* indices_ptr = vector_at(data->indices, i);
        ObjIndex combined_idx;
        uintptr_t val;
        if (hashmap_get(indices_map, indices_ptr, sizeof(ObjIndices), &val)) {
            combined_idx = (ObjIndex)val;
        } else {
            combined_idx = next_combined_idx;
            hashmap_set(indices_map, indices_ptr, sizeof(ObjIndices), combined_idx);
            if (!hashmap_ok(indices_map)) {
                hashmap_free(indices_map);
                LOG_ERR(kErrorAllocationFailed);
                return kErrorAllocationFailed;
            }
            const unsigned int i_v = indices_ptr->f * 3, i_n = indices_ptr->n * 3, i_t = indices_ptr->t * 2;
            void* vert_ptr = vector_push(object->vertices, 3);
            if (vert_ptr == NULL) {
                hashmap_free(indices_map);
                LOG_ERR(kErrorAllocationFailed);
                return kErrorAllocationFailed;
            }
            memcpy(vert_ptr, vector_at(data->verticies.v, i_v), 3 * sizeof(ObjCoord));
            if (data->verticies.t->size != 0) {
                vert_ptr = vector_push(object->vertices, 2);
                if (vert_ptr == NULL) {
                    hashmap_free(indices_map);
                    LOG_ERR(kErrorAllocationFailed);
                    return kErrorAllocationFailed;
                }
                memcpy(vert_ptr, vector_at(data->verticies.t, i_t), 2 * sizeof(ObjCoord));
            }
            if (data->verticies.n->size != 0) {
                vert_ptr = vector_push(object->vertices, 3);
                if (vert_ptr == NULL) {
                    hashmap_free(indices_map);
                    LOG_ERR(kErrorAllocationFailed);
                    return kErrorAllocationFailed;
                }
                memcpy(vert_ptr, vector_at(data->verticies.n, i_n), 3 * sizeof(ObjCoord));
            }
            ++next_combined_idx;
        }
        ObjIndex* index_ptr = vector_push(object->indices, 1);
        if (index_ptr == NULL) {
            hashmap_free(indices_map);
            LOG_ERR(kErrorAllocationFailed);
            return kErrorAllocationFailed;
        }
        *index_ptr = combined_idx;
    }
    hashmap_free(indices_map);
    return kErrorNil;
}

error render_object_create(RenderObject* object, ObjData* data) {
    object->indices = vector_create(sizeof(ObjIndex), data->indices->size);
    if (object->indices == NULL) {
        LOG_ERR(kErrorAllocationFailed);
        return kErrorAllocationFailed;
    }
    object->vertices = vector_create(sizeof(ObjCoord), data->indices->size);
    if (object->vertices == NULL) {
        LOG_ERR(kErrorAllocationFailed);
        return kErrorAllocationFailed;
    }
    error err = optimize_geometry(object, data);
    if (err != kErrorNil) {
        LOG_ERR(err);
        return err;
    }
    vector* maps = vector_create(sizeof(unsigned int), 0);
    if (maps == NULL) {
        LOG_ERR(kErrorAllocationFailed);
        return kErrorAllocationFailed;
    }
    err = maps_create(data->dir_path, data->mtl, &maps);
    if (err != kErrorNil) {
        LOG_ERR(err);
        return err;
    }
    object->maps = maps;
    object->usemtl = data->usemtl;

    data->usemtl = NULL;
    data->mtl = NULL;

    return kErrorNil;
}

void render_object_free(const RenderObject* object) {
    if (object == NULL) {
        return;
    }
    vector_free(object->indices);
    vector_free(object->vertices);
    vector_free(object->usemtl);
    if (object->maps == NULL) {
        return;
    }
    for (size_t i = 0; i < object->maps->size; ++i) {
        map_free(vector_at(object->maps, i));
    }
    vector_free(object->maps);
}