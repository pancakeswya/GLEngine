#ifndef ENGINE_RENDER_CONTEXT_H_
#define ENGINE_RENDER_CONTEXT_H_

#include "base/error.h"
#include "engine/render/object.h"

#include <GL/glew.h>

typedef struct RenderContext {
    RenderObject* object;

    GLuint program;
    GLuint vao;
    GLuint u_transform;
} RenderContext;

error render_context_create(const RenderObject* object, RenderContext* context);
void render_context_free(const RenderContext* context);

#endif // ENGINE_RENDER_CONTEXT_H_