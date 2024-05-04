#include "obj/data.h"
#include "log/log.h"

#include <stdlib.h>

error obj_data_create(ObjData* data) {
    data->dir_path = NULL;

    data->mtl = vector_create(sizeof(ObjNewMtl), 0);
    if (data->mtl == NULL) {
        LOG_ERR(kErrorAllocationFailed);
        return kErrorAllocationFailed;
    }
    data->usemtl = vector_create(sizeof(ObjUseMtl), 0);
    if (data->usemtl == NULL) {
        LOG_ERR(kErrorAllocationFailed);
        return kErrorAllocationFailed;
    }
    data->indices = vector_create(sizeof(ObjIndices), 0);
    if (data->indices == NULL) {
        LOG_ERR(kErrorAllocationFailed);
        return kErrorAllocationFailed;
    }
    data->verticies.v = vector_create(sizeof(ObjCoord), 0);
    if (data->verticies.v == NULL) {
        LOG_ERR(kErrorAllocationFailed);
        return kErrorAllocationFailed;
    }
    data->verticies.n = vector_create(sizeof(ObjCoord), 0);
    if (data->verticies.n == NULL) {
        LOG_ERR(kErrorAllocationFailed);
        return kErrorAllocationFailed;
    }
    data->verticies.t = vector_create(sizeof(ObjCoord), 0);
    if (data->verticies.t == NULL) {
        LOG_ERR(kErrorAllocationFailed);
        return kErrorAllocationFailed;
    }
    return kErrorNil;
}

void obj_data_free(const ObjData *data) {
    if (data->mtl) {
        for (size_t i = 0; i < data->mtl->size; ++i) {
            const ObjNewMtl* mtl_ptr = vector_at(data->mtl, i);
            free(mtl_ptr->name);
            free(mtl_ptr->map_kd);
            free(mtl_ptr->map_Ns);
            free(mtl_ptr->map_bump);
        }
        vector_free(data->mtl);
    }
    if (data->usemtl) {
        vector_free(data->usemtl);
    }
    vector_free(data->indices);

    vector_free(data->verticies.v);
    vector_free(data->verticies.n);
    vector_free(data->verticies.t);

    free(data->dir_path);
}