#include "engine/render/context.h"
#include "base/io.h"

#include <stdlib.h>

static error compile_shader_from_file(const char* path, const GLuint shader) {
  char* content = NULL;
  const error err = read_file(path, &content, NULL);
  if (err != kErrorNil) {
    return err;
  }
  const char* source = content;
  glShaderSource(shader, 1, &source, NULL);
  glCompileShader(shader);
  free(content);

  GLint status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE) {
    return kErrorShaderCompiling;
  }
  return kErrorNil;
}

static error link_shader_program(const GLuint program) {
  glLinkProgram(program);
  GLint status;
  glGetProgramiv(program, GL_LINK_STATUS, &status);
  if (status == GL_FALSE) {
    return kErrorProgramLinking;
  }
  return kErrorNil;
}

#include <stdio.h>

error render_context_create(RenderObject* object, RenderContext* context) {
  glEnable(GL_DEPTH_TEST);

  glGenVertexArrays(1, &context->vao);
  glBindVertexArray(context->vao);

  GLuint triangles_ebo;
  glGenBuffers(1, &triangles_ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangles_ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, object->indices->size * object->indices->data_size, object->indices, GL_STATIC_DRAW);

  GLuint verticies_vbo;
  glGenBuffers(1, &verticies_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, verticies_vbo);
  glBufferData(GL_ARRAY_BUFFER, object->vertices->size * object->vertices->data_size, object->vertices, GL_STATIC_DRAW);

  const GLuint verticies_index = 0;
  glVertexAttribPointer(verticies_index, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(verticies_index);

  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  const GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  error err = compile_shader_from_file("/mnt/c/Users/user/CLionProjects/untitled3/shaders/vertex.glsl", vertex_shader);
  if (err != kErrorNil) {
    return err;
  }
  const GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  err = compile_shader_from_file("/mnt/c/Users/user/CLionProjects/untitled3/shaders/fragment.glsl", fragment_shader);
  if (err != kErrorNil) {
    return err;
  }
  context->program = glCreateProgram();
  glAttachShader(context->program, vertex_shader);
  glAttachShader(context->program, fragment_shader);
  err = link_shader_program(context->program);
  if (err != kErrorNil) {
    return err;
  }
  context->u_transform = glGetUniformLocation(context->program, "transform");

  context->object = object;
  return kErrorNil;
}

void render_context_free(const RenderContext* context) {
  render_object_free(context->object);
}
