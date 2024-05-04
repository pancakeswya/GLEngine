#ifndef BASE_ERROR_H_
#define BASE_ERROR_H_

typedef enum {
    kErrorNil = 0,
    kErrorAllocationFailed,
    kErrorNoFileFound,
    kErrorReadingFile,
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

#endif // BASE_ERROR_H_