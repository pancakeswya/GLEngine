#ifndef OBJ_DATA_H_
#define OBJ_DATA_H_

#include "base/error.h"
#include "base/vector.h"

typedef unsigned int ObjIndex_t;
typedef float ObjCoord_t;

typedef struct ObjIndices {
    ObjIndex_t f, n, t;
} ObjIndices;

typedef struct ObjNewMtl {
    float Ns;
    float Ni;
    float d;
    float Ka[3];
    float Kd[3];
    float Ks[3];
    float Ke[3];

    char *name;
    char *map_kd;
    char *map_Ns;
    char *map_bump;

    long int illum;
} ObjNewMtl;

typedef struct ObjUseMtl {
    unsigned int index;
    unsigned int offset;
} ObjUseMtl;

typedef struct ObjData {
    vector* mtl;
    vector* usemtl;

    vector* indices;

    struct {
        vector* v;
        vector* n;
        vector* t;
    } verticies;

    char *dir_path;
} ObjData;

error obj_data_init(ObjData* data);
void obj_data_free(const ObjData *data);

#endif // OBJ_DATA_H_