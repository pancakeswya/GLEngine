#ifndef RENDER_H_
#define RENDER_H_

#include "base/error.h"

#include <GL/glew.h>

typedef struct {
    GLfloat* vertices;
    GLfloat* colors;
    GLuint* indices;

    size_t verticies_size;
    size_t colors_size;
    size_t indices_size;
} RenderObject;

typedef struct {
    GLuint program;
    GLuint vao;
    GLuint u_transform;
} RenderContext;

error render_context_create(const RenderObject* object, RenderContext* context);
void render(const RenderContext* context);

#endif // RENDER_H_