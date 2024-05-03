#include "engine/render.h"
#include "base/io.h"
#include "math/matrix.h"
#include "log/log.h"

#include <GLFW/glfw3.h>
#include <stdlib.h>

static inline float animation(const float duration) {
  const unsigned long int ms_time = glfwGetTime() * 1000;
  const unsigned int ms_duration = duration * 1000;
  const float ms_position = ms_time % ms_duration;
  return ms_position / ms_duration;
}

static error compile_shader_from_file(const char* path, const GLuint shader) {
  char* content = NULL;
  const error err = read_file(path, &content);
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
#ifdef DEBUG
    LOG_GL(shader);
#endif
    return kErrorShaderCompiling;
  }
  return kErrorNil;
}

static error link_shader_program(const GLuint program) {
  glLinkProgram(program);
  GLint status;
  glGetProgramiv(program, GL_LINK_STATUS, &status);
  if (status == GL_FALSE) {
#ifdef DEBUG
    LOG_GL(program);
#endif
    return kErrorProgramLinking;
  }
  return kErrorNil;
}

error render_context_create(const RenderObject* object, RenderContext* context) {
  glEnable(GL_DEPTH_TEST);

  glGenVertexArrays(1, &context->vao);
  glBindVertexArray(context->vao);

  GLuint triangles_ebo;
  glGenBuffers(1, &triangles_ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangles_ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, object->indices_size, object->indices, GL_STATIC_DRAW);

  GLuint verticies_vbo;
  glGenBuffers(1, &verticies_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, verticies_vbo);
  glBufferData(GL_ARRAY_BUFFER, object->verticies_size, object->vertices, GL_STATIC_DRAW);

  const GLuint verticies_index = 0;
  glVertexAttribPointer(verticies_index, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(verticies_index);

  GLuint colors_vbo;
  glGenBuffers(1, &colors_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
  glBufferData(GL_ARRAY_BUFFER, object->colors_size, object->colors, GL_STATIC_DRAW);

  const GLuint colors_index = 1;
  glVertexAttribPointer(colors_index, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(colors_index);

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
  return kErrorNil;
}

void render(const RenderContext* context) {
  const GLuint triangles = 6 * 2;

  glClearColor(0.1, 0.12, 0.2, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glUseProgram(context->program);

  mat4f transform = mat4f_identity;
  transform = mat4f_multiply(transform, mat4f_perspective(0.5, 0.5, 1, 5));
  transform = mat4f_multiply(transform, mat4f_translation(0, 0, -3));
  transform = mat4f_multiply(transform, mat4f_rotate_x(0.15 * pi));
  transform = mat4f_multiply(transform, mat4f_rotate_y(2 * pi * animation(4)));
  glUniformMatrix4fv(context->u_transform, 1, GL_FALSE, transform.data);

  glBindVertexArray(context->vao);
  glDrawElements(GL_TRIANGLES, triangles * 3, GL_UNSIGNED_INT, NULL);
}