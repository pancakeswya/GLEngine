#include "engine/render/render.h"

#include "math/matrix.h"

#include <GLFW/glfw3.h>

static inline float animation(const float duration) {
  const unsigned long int ms_time = glfwGetTime() * 1000;
  const unsigned int ms_duration = duration * 1000;
  const float ms_position = ms_time % ms_duration;
  return ms_position / ms_duration;
}

void render(const RenderContext* context) {
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
  glDrawElements(GL_TRIANGLES, context->object->indices->size, GL_UNSIGNED_INT, NULL);
}