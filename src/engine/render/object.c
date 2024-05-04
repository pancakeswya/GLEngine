#include "engine/render/object.h"
#include "base/map.h"
#include "log/log.h"

#include <string.h>

error render_object_create(RenderObject* object, ObjData* data) {
    vector* indices = vector_create(sizeof(ObjIndex), data->indices->size);
    if (indices == NULL) {
        LOG_ERR(kErrorAllocationFailed);
        return kErrorAllocationFailed;
    }
    vector* vertices = vector_create(sizeof(ObjCoord), data->indices->size);
    if (vertices == NULL) {
        LOG_ERR(kErrorAllocationFailed);
        return kErrorAllocationFailed;
    }
    hashmap* indices_map = hashmap_create();
    if (indices_map == NULL) {
        LOG_ERR(kErrorAllocationFailed);
        return kErrorAllocationFailed;
    }
    error err = kErrorNil;
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
                LOG_ERR(kErrorAllocationFailed);
                err = kErrorAllocationFailed;
                goto cleanup;
            }
            const unsigned int i_v = indices_ptr->f * 3, i_n = indices_ptr->n * 3, i_t = indices_ptr->t * 2;
            void* vert_ptr = vector_push(vertices, 3);
            if (vert_ptr == NULL) {
                LOG_ERR(kErrorAllocationFailed);
                err = kErrorAllocationFailed;
                goto cleanup;
            }
            memcpy(vert_ptr, vector_at(data->verticies.v, i_v), 3 * sizeof(ObjCoord));
            if (data->verticies.t->size != 0) {
                vert_ptr = vector_push(vertices, 2);
                if (vert_ptr == NULL) {
                    LOG_ERR(kErrorAllocationFailed);
                    err = kErrorAllocationFailed;
                    goto cleanup;
                }
                memcpy(vert_ptr, vector_at(data->verticies.t, i_t), 2 * sizeof(ObjCoord));
            }
            if (data->verticies.n->size != 0) {
                vert_ptr = vector_push(vertices, 3);
                if (vert_ptr == NULL) {
                    LOG_ERR(kErrorAllocationFailed);
                    err = kErrorAllocationFailed;
                    goto cleanup;
                }
                memcpy(vert_ptr, vector_at(data->verticies.n, i_n), 3 * sizeof(ObjCoord));
            }
            ++next_combined_idx;
        }
        ObjIndex* index_ptr = vector_push(indices, 1);
        if (index_ptr == NULL) {
            LOG_ERR(kErrorAllocationFailed);
            err = kErrorAllocationFailed;
            goto cleanup;
        }
        *index_ptr = combined_idx;
    }
    *object = (RenderObject) {
        .indices = indices,
        .vertices = vertices,
        .usemtl = data->usemtl,
        .mtl = data->mtl
    };
    data->usemtl = NULL;
    data->mtl = NULL;
cleanup:
    hashmap_free(indices_map);
    return err;
}

void render_object_free(const RenderObject* object) {
    if (object == NULL) {
        return;
    }
    vector_free(object->indices);
    vector_free(object->vertices);
    vector_free(object->usemtl);
    vector_free(object->mtl);
}