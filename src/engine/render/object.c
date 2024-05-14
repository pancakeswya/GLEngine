#include "engine/render/object.h"
#include "engine/render/maps.h"
#include "obj/parser.h"
#include "hashmap/hashmap.h"
#include "log/log.h"

#include <string.h>

typedef struct HashmapNode {
  ObjIndices* indices;
  ObjIndex index;
} HashmapNode;

static int indices_compare(const void *a, const void *b, __attribute__((unused)) void *udata) {
    const ObjIndices* lhs = ((HashmapNode*)a)->indices;
    const ObjIndices* rhs = ((HashmapNode*)b)->indices;
    if (lhs->f < rhs->f) return 1;
    if (rhs->f < lhs->f) return 0;
    if (lhs->n < rhs->n) return 1;
    if (rhs->n < lhs->n) return 0;
    if (lhs->t < rhs->t) return 1;
    return rhs->t < lhs->t;
}

static uint64_t indices_hash(const void *item, const uint64_t seed0, const uint64_t seed1) {
    const HashmapNode* node = item;
    return hashmap_sip(node->indices, sizeof(ObjIndices), seed0, seed1);
}

static error move_and_optimize_geometry(const RenderObject* object, const ObjData* data) {
    struct hashmap* indices_map = hashmap_new(sizeof(HashmapNode), 0, 0, 0,
                                     indices_hash, indices_compare, NULL, NULL);
    if (indices_map == NULL) {
        LOG_ERR(kErrorAllocationFailed);
        return kErrorAllocationFailed;
    }
    ObjIndex next_combined_idx = 0, combined_idx;
    ObjIndices* indices = data->indices->data;
    for(size_t i = 0; i < data->indices->size; ++i) {
        const HashmapNode* curr_node = hashmap_get(indices_map, &(HashmapNode){
          .indices = indices + i
        });
        if (curr_node != NULL) {
            combined_idx = curr_node->index;
        } else {
            combined_idx = next_combined_idx;
            hashmap_set(indices_map, &(HashmapNode) {
              .indices = indices + i,
              .index = combined_idx
            });
            if (hashmap_oom(indices_map)) {
                hashmap_free(indices_map);
                LOG_ERR(kErrorAllocationFailed);
                return kErrorAllocationFailed;
            }
            const unsigned int i_v = indices[i].f * 3, i_n = indices[i].n * 3, i_t = indices[i].t * 2;
            void* vert_ptr = vector_push(object->vertices, 3);
            if (vert_ptr == NULL) {
                hashmap_free(indices_map);
                LOG_ERR(kErrorAllocationFailed);
                return kErrorAllocationFailed;
            }
            memcpy(vert_ptr, vector_at(data->verticies.v, i_v), 3 * sizeof(ObjCoord));
            if (data->verticies.n->size != 0) {
                vert_ptr = vector_push(object->vertices, 3);
                if (vert_ptr == NULL) {
                    hashmap_free(indices_map);
                    LOG_ERR(kErrorAllocationFailed);
                    return kErrorAllocationFailed;
                }
                memcpy(vert_ptr, vector_at(data->verticies.n, i_n), 3 * sizeof(ObjCoord));
            }
            if (data->verticies.t->size != 0) {
                vert_ptr = vector_push(object->vertices, 2);
                if (vert_ptr == NULL) {
                    hashmap_free(indices_map);
                    LOG_ERR(kErrorAllocationFailed);
                    return kErrorAllocationFailed;
                }
                memcpy(vert_ptr, vector_at(data->verticies.t, i_t), 2 * sizeof(ObjCoord));
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

error render_object_create(const char* path, RenderObject* object) {
    ObjData data = {0};
    error err = obj_data_create(&data);
    if (err != kErrorNil) {
        LOG_ERR(err);
        return err;
    }
    err = obj_data_parse(path, &data);
    if (err != kErrorNil) {
        LOG_ERR(err);
        goto cleanup;
    }
    object->indices = vector_create(sizeof(ObjIndex), data.indices->size);
    if (object->indices == NULL) {
        LOG_ERR(kErrorAllocationFailed);
        goto cleanup;
    }
    object->vertices = vector_create(sizeof(ObjCoord), data.indices->size);
    if (object->vertices == NULL) {
        LOG_ERR(kErrorAllocationFailed);
        goto cleanup;
    }
    err = move_and_optimize_geometry(object, &data);
    if (err != kErrorNil) {
        LOG_ERR(err);
        goto cleanup;
    }
    object->maps = vector_create(sizeof(RenderMaps), 0);
    if (object->maps == NULL) {
        LOG_ERR(kErrorAllocationFailed);
        goto cleanup;
    }
    err = maps_create(data.dir_path, data.mtl, &object->maps);
    if (err != kErrorNil) {
        LOG_ERR(err);
        goto cleanup;
    }
    object->usemtl = data.usemtl;
    data.usemtl = NULL;
cleanup:
    obj_data_free(&data);
    return err;
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
    const RenderMaps* maps = object->maps->data;
    for (size_t i = 0; i < object->maps->size; ++i) {
        map_free(maps +i);
    }
    vector_free(object->maps);
}