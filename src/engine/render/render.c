#include "engine/render/render.h"
#include "log/log.h"
#include "obj/parser.h"
#include "math/matrix.h"

#include <GLFW/glfw3.h>
#include <stdlib.h>

static inline float animation(const float duration) {
  const unsigned long int ms_time = glfwGetTime() * 1000;
  const unsigned int ms_duration = duration * 1000;
  const float ms_position = ms_time % ms_duration;
  return ms_position / ms_duration;
}

error render_init(RenderContext* context) {
  ObjData data;
  error err = obj_data_create(&data);
  if (err != kErrorNil) {
    LOG_ERR(err);
    return err;
  }
  err = obj_data_parse("/mnt/c/Users/user/CLionProjects/ObjViewer_v2.0/obj/cube2.obj", &data);
  if (err != kErrorNil) {
    LOG_ERR(err);
    goto cleanup;
  }
  RenderObject* object = (RenderObject*)malloc(sizeof *object);
  if (object == NULL) {
    LOG_ERR(kErrorAllocationFailed);
    err = kErrorAllocationFailed;
    goto cleanup;
  }
  err = render_object_create(object, &data);
  if (err != kErrorNil) {
    LOG_ERR(err);
    goto cleanup;
  }
  obj_data_free(&data);
  return render_context_create(object, context);
cleanup:
  obj_data_free(&data);
  return err;
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