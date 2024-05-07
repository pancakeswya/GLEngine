#include "base/config.h"
#include "log/log.h"
#include "ini/ini.h"

#include <stdlib.h>
#include <string.h>

#define MATCH(s, n)

static char* config_strdup(const char *str) {
    const size_t len = strlen(str) + 1;
    char* res = (char*)malloc(len * sizeof(char));
    if (res == NULL) {
        return NULL;
    }
    memcpy(res, str, len* sizeof(char));
    res[len - 1] = '\0';
    return res;
}

static int config_handler(void* conf_ptr, const char* section, const char* name,
                   const char* value) {
    Config* config = (Config*)conf_ptr;
    if (strcmp(section, "render") == 0 && strcmp(name, "shader_v") == 0) {
        config->render.shader_paths.shader_v = config_strdup(value);
        if (config->render.shader_paths.shader_v == NULL) {
            LOG_ERR(kErrorAllocationFailed);
            return 0;
        }
    } else if (strcmp(section, "render") == 0 && strcmp(name, "shader_f") == 0) {
        config->render.shader_paths.shader_f = config_strdup(value);
        if (config->render.shader_paths.shader_f == NULL) {
            LOG_ERR(kErrorAllocationFailed);
            return 0;
        }
    } else if (strcmp(section, "render") == 0 && strcmp(name, "objects[]") == 0) {
        char* object_path = config_strdup(value);
        if (object_path == NULL) {
            LOG_ERR(kErrorAllocationFailed);
            return 0;
        }
        char** object_paths_ptr = vector_push(config->render.objects_paths, 1);
        if (object_paths_ptr == NULL) {
            LOG_ERR(kErrorAllocationFailed);
            return 0;
        }
        *object_paths_ptr = object_path;
    } else if (strcmp(section, "window") == 0 && strcmp(name, "name") == 0) {
        config->window.name = config_strdup(value);
        if (config->window.name == NULL) {
            LOG_ERR(kErrorAllocationFailed);
            return 0;
        }
    } else if (strcmp(section, "window") == 0 && strcmp(name, "width") == 0) {
        config->window.width = atoi(value);
    } else if (strcmp(section, "window") == 0 && strcmp(name, "height") == 0) {
        config->window.height = atoi(value);
    } else {
        LOG_ERR(kErrorParsingConfig);
        return 0;
    }
    return 1;
}

error config_open(const char* path, Config* config) {
    config->render.objects_paths = vector_create(sizeof(char*), 0);
    if (config->render.objects_paths == NULL) {
        LOG_ERR(kErrorAllocationFailed);
        return kErrorAllocationFailed;
    }
    if (ini_parse(path, config_handler, config) != 0) {
        LOG_ERR(kErrorParsingConfig);
        return kErrorParsingConfig;
    }
    return kErrorNil;
}

void config_close(const Config* config) {
    if (config == NULL) {
        return;
    }
    free(config->window.name);
    free(config->render.shader_paths.shader_v);
    free(config->render.shader_paths.shader_f);
    if (config->render.objects_paths == NULL) {
        return;
    }
    char** object_paths = config->render.objects_paths->data;
    for(size_t i = 0; i < config->render.objects_paths->size; ++i) {
        free(object_paths[i]);
    }
    vector_free(config->render.objects_paths);
}