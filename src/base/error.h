#ifndef ERROR_H_
#define ERROR_H_

typedef enum {
    kErrorNil = 0,
    kErrorAllocationFailed,
    kErrorNoShaderFileFound,
    kErrorReadingShaderFile,
    kErrorShaderCompiling,
    kErrorProgramLinking,
    kErrorGlfwInit,
    kErrorWindowCreate,
    kErrorGlewInit,
    kErrorReadingObjFile,
    kErrorReadingMtlFile,
    kErrorNotSupportedGeometry
} error;

const char* error_str(error err);

#endif // ERROR_H_