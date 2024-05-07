#ifndef BASE_CONFIG
#define BASE_CONFIG

#include "base/error.h"
#include "base/vector.h"

typedef struct WindowConfig {
    int width, height;
    char* name;
} WindowConfig;

typedef struct ShaderPaths {
    char* shader_v;
    char* shader_f;
} ShaderPaths;

typedef struct RenderConfig {
    vector* objects_paths;
    ShaderPaths shader_paths;
} RenderConfig;

typedef struct Config {
    RenderConfig render;
    WindowConfig window;
} Config;

error config_open(const char* path, Config* config);
void config_close(const Config* config);


#endif // BASE_CONFIG