#include "error.h"

const char* error_str(const error err) {
    static const char* err_list[] = {
        "No error occurred",
        "Allocation fail",
        "No file found",
        "Cant read file",
        "Cant compile shader",
        "Cant link program",
        "Cant init glfw",
        "Cant create the window",
        "Cant init glew",
        "Cant read obj file",
        "Cant read mtl file",
        "Not supported geometry"
    };
    return err_list[err];
}
