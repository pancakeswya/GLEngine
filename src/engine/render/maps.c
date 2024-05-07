#include "engine/render/maps.h"
#include "base/strutil.h"
#include "obj/data.h"
#include "log/log.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

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
    unsigned char* image = stbi_load(path, &width, &height, &n_channels, STBI_rgb);
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

void map_free(const RenderMaps* maps) {
    stbi_image_free(maps->kd.image);
    stbi_image_free(maps->ns.image);
    stbi_image_free(maps->bump.image);
}

error maps_create(const char* dir_path, const vector* mtl, vector** maps_ptr) {
    vector* maps = *maps_ptr;
    const ObjNewMtl* mtl_data = mtl->data;
    stbi_set_flip_vertically_on_load(true);
    for (size_t i = 0; i < mtl->size; ++i) {
        RenderMaps r_maps = {0};
        error err = load_map(dir_path, mtl_data[i].map_kd, &r_maps.kd);
        if (err != kErrorNil) {
            LOG_ERR(err);
            return err;
        }
        err = load_map(dir_path, mtl_data[i].map_Ns, &r_maps.ns);
        if (err != kErrorNil) {
            map_free(&r_maps);
            LOG_ERR(err);
            return err;
        }
        err = load_map(dir_path, mtl_data[i].map_bump, &r_maps.bump);
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