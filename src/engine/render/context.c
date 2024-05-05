#include "engine/render/context.h"
#include "base/io.h"
#include "log/log.h"

#include <stdlib.h>

static error compile_shader_from_file(const char* path, const GLuint shader) {
  char* content = NULL;
  const error err = read_file(path, &content, NULL);
  if (err != kErrorNil) {
    LOG_ERR(err);
    return err;
  }
  const char* source = content;
  glShaderSource(shader, 1, &source, NULL);
  glCompileShader(shader);
  free(content);

  GLint status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE) {
    LOG_GL(shader);
    return kErrorShaderCompiling;
  }
  return kErrorNil;
}

static error link_shader_program(const GLuint program) {
  glLinkProgram(program);
  GLint status;
  glGetProgramiv(program, GL_LINK_STATUS, &status);
  if (status == GL_FALSE) {
    LOG_GL(program);
    return kErrorProgramLinking;
  }
  return kErrorNil;
}

error render_context_create(const RenderObject* object, RenderContext* context) {
  context->object = (RenderObject*)malloc(sizeof(RenderObject));
  if (context->object == NULL) {
    LOG_ERR(kErrorAllocationFailed);
    return kErrorAllocationFailed;
  }
  *context->object = *object;

  glEnable(GL_DEPTH_TEST);

  glGenVertexArrays(1, &context->vao);
  glBindVertexArray(context->vao);

  GLuint triangles_ebo;
  glGenBuffers(1, &triangles_ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangles_ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, object->indices->size * object->indices->data_size, object->indices->data, GL_STATIC_DRAW);

  GLuint verticies_vbo;
  glGenBuffers(1, &verticies_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, verticies_vbo);
  glBufferData(GL_ARRAY_BUFFER, object->vertices->size * object->vertices->data_size, object->vertices->data, GL_STATIC_DRAW);

  const GLuint verticies_index = 0;
  glVertexAttribPointer(verticies_index, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(verticies_index);

  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  const GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  error err = compile_shader_from_file("/mnt/c/Users/user/CLionProjects/GLEngine/shaders/vertex.glsl", vertex_shader);
  if (err != kErrorNil) {
    LOG_ERR(err);
    return err;
  }
  const GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  err = compile_shader_from_file("/mnt/c/Users/user/CLionProjects/GLEngine/shaders/fragment.glsl", fragment_shader);
  if (err != kErrorNil) {
    LOG_ERR(err);
    return err;
  }
  context->program = glCreateProgram();
  glAttachShader(context->program, vertex_shader);
  glAttachShader(context->program, fragment_shader);
  err = link_shader_program(context->program);
  if (err != kErrorNil) {
    LOG_ERR(err);
    return err;
  }
  context->u_transform = glGetUniformLocation(context->program, "transform");

  return kErrorNil;
}

void render_context_free(const RenderContext* context) {
  if (context->object == NULL) {
    return;
  }
  render_object_free(context->object);
  free(context->object);
}
