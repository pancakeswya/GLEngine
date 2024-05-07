#include "engine/render/render.h"
#include "base/config.h"
#include "log/log.h"
#include "obj/parser.h"
#include "math/matrix.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

static inline float animation(const float duration) {
  const unsigned long int ms_time = glfwGetTime() * 1000;
  const unsigned int ms_duration = duration * 1000;
  const float ms_position = ms_time % ms_duration;
  return ms_position / ms_duration;
}

error render_init(const RenderConfig* config, RenderContext* context) {
  ObjData data = {0};
  RenderObject object = {0};
  error err = obj_data_create(&data);
  if (err != kErrorNil) {
    LOG_ERR(err);
    goto fail;
  }
  const char* object_path = *(char**)config->objects_paths->data;
  err = obj_data_parse(object_path, &data);
  if (err != kErrorNil) {
    LOG_ERR(err);
    goto fail;
  }
  err = render_object_create(&object, &data);
  if (err != kErrorNil) {
    LOG_ERR(err);
    goto fail;
  }
  err = render_context_create(&config->shader_paths, &object, context);
  if (err != kErrorNil) {
    LOG_ERR(err);
    goto fail;
  }
  obj_data_free(&data);
  return kErrorNil;
fail:
  obj_data_free(&data);
  render_object_free(&object);
  return err;
}

void render(const RenderContext* context) {
  glClearColor(0.1, 0.12, 0.2, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glUseProgram(context->program);
  glBindVertexArray(context->vao);

  mat4f transform = mat4f_identity;
  transform = mat4f_multiply(transform, mat4f_perspective(0.5, 0.5, 1, 5));
  transform = mat4f_multiply(transform, mat4f_translation(0, 0, -3));
  transform = mat4f_multiply(transform, mat4f_rotate_x(0.15 * pi));
  transform = mat4f_multiply(transform, mat4f_rotate_y(2 * pi * animation(4)));
  glUniformMatrix4fv(context->uniforms.transform, 1, GL_FALSE, transform.data);

  const RenderMapsTextures* maps = context->maps->data;
  size_t prev_offset = 0;
  ObjUseMtl* use_mtl = context->object->usemtl->data;

  for(size_t i = 0; i < context->object->usemtl->size; ++i) {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, maps[use_mtl[i].index].map_kd);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, maps[use_mtl[i].index].map_ns);
      glActiveTexture(GL_TEXTURE2);
      glBindTexture(GL_TEXTURE_2D, maps[use_mtl[i].index].map_bump);

      glDrawElements(GL_TRIANGLES, use_mtl[i].offset - prev_offset,
                   GL_UNSIGNED_INT,
                   (void*)(prev_offset * sizeof(GLuint)));
      prev_offset = use_mtl[i].offset;
  }
}