#ifndef ENGINE_RENDER_CONTEXT_H_
#define ENGINE_RENDER_CONTEXT_H_

#include "base/error.h"
#include "base/vector.h"
#include "engine/render/object.h"

#include <GL/glew.h>

typedef struct RenderMapsTextures {
    unsigned int map_kd, map_ns, map_bump;
} RenderMapsTextures;

typedef struct RenderUniforms {
  GLuint transform;
} RenderUniforms;

typedef struct RenderContext {
    RenderObject* object;
    RenderUniforms uniforms;

    vector* maps;

    GLuint program;
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
} RenderContext;

error render_context_create(const RenderObject* object, RenderContext* context);
void render_context_free(const RenderContext* context);

#endif // ENGINE_RENDER_CONTEXT_H_