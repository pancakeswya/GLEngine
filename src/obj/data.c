#include "obj/data.h"

#include <stdlib.h>

error obj_data_init(ObjData* data) {
    data->dir_path = NULL;

    data->mtl = vector_init(sizeof(ObjNewMtl), 0);
    if (data->mtl == NULL) {
        return kErrorAllocationFailed;
    }
    data->usemtl = vector_init(sizeof(ObjUseMtl), 0);
    if (data->usemtl == NULL) {
        return kErrorAllocationFailed;
    }
    data->indices = vector_init(sizeof(ObjIndices), 0);
    if (data->indices == NULL) {
        return kErrorAllocationFailed;
    }
    data->verticies.v = vector_init(sizeof(ObjCoord_t), 0);
    if (data->verticies.v == NULL) {
        return kErrorAllocationFailed;
    }
    data->verticies.n = vector_init(sizeof(ObjCoord_t), 0);
    if (data->verticies.n == NULL) {
        return kErrorAllocationFailed;
    }
    data->verticies.t = vector_init(sizeof(ObjCoord_t), 0);
    if (data->verticies.t == NULL) {
        return kErrorAllocationFailed;
    }
    return kErrorNil;
}

void obj_data_free(const ObjData *data) {
    for (size_t i = 0; i < data->mtl->size; ++i) {
        const ObjNewMtl* mtl_ptr = vector_at(data->mtl, i);
        free(mtl_ptr->name);
        free(mtl_ptr->map_kd);
        free(mtl_ptr->map_Ns);
        free(mtl_ptr->map_bump);
    }
    vector_free(data->mtl);
    vector_free(data->usemtl);
    vector_free(data->indices);

    vector_free(data->verticies.v);
    vector_free(data->verticies.n);
    vector_free(data->verticies.t);

    free(data->dir_path);
}