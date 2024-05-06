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

static unsigned int map_texture_create(const RenderMap* map) {
  unsigned int texture;

  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, map->width, map->height, 0, GL_RGB, GL_UNSIGNED_BYTE, map->image);
  glGenerateMipmap(GL_TEXTURE_2D);

  return texture;
}

error render_context_create(const RenderObject* object, RenderContext* context) {
  static const size_t stride = 8 * sizeof(float);

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

  static const GLuint verticies_index = 0;
  glVertexAttribPointer(verticies_index, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
  glEnableVertexAttribArray(verticies_index);

  static const GLuint normal_index = 1;
  glVertexAttribPointer(normal_index, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(normal_index);

  static const GLuint texture_index = 2;
  glVertexAttribPointer(texture_index, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(texture_index);

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
  context->u_transform = glGetUniformLocation(context->program, "u_transform");
  context->maps = vector_create(sizeof(RenderMapsTextures), object->maps->size * render_map_count);
  if (context->maps == NULL) {
    LOG_ERR(kErrorAllocationFailed);
    return kErrorAllocationFailed;
  }
  const RenderMaps* maps = object->maps->data;
  for(size_t i = 0; i < object->maps->size; ++i) {
    RenderMapsTextures* map_textures_ptr = vector_push(context->maps, 1);
    if (map_textures_ptr == NULL) {
      LOG_ERR(kErrorAllocationFailed);
      return kErrorAllocationFailed;
    }
    map_textures_ptr->map_kd = map_texture_create(&maps[i].kd);
    map_textures_ptr->map_ns = map_texture_create(&maps[i].ns);
    map_textures_ptr->map_bump = map_texture_create(&maps[i].bump);
  }

  return kErrorNil;
}

void render_context_free(const RenderContext* context) {
  if (context->object == NULL) {
    return;
  }
  render_object_free(context->object);
  vector_free(context->maps);
  free(context->object);
}
