#include "engine/window.h"
#include "engine/render/render.h"
#include "base/error.h"
#include "obj/parser.h"

#include <stdio.h>
#include <stdlib.h>

static void update_fps(GLFWwindow* window) {
  static double last_update_time = 0;
  static int frames_since_last_update = 0;

  const double now = glfwGetTime();
  frames_since_last_update++;

  if (now - last_update_time > 0.25) {
    const double fps = frames_since_last_update / (now - last_update_time);
    char title_buffer[128];
    sprintf(title_buffer, "(%.1f FPS)", fps);
    glfwSetWindowTitle(window, title_buffer);
    last_update_time = now;
    frames_since_last_update = 0;
  }
}

error window_create(const int width, const int height, const char* title, WindowContext* context) {
  if (glfwInit() == GLFW_FALSE) {
    return kErrorGlfwInit;
  }
  GLFWmonitor* fullscreen_monitor = NULL;
  GLFWwindow* share = NULL;
  GLFWwindow* window = glfwCreateWindow(width, height, title, fullscreen_monitor, share);
  if (window == NULL) {
    return kErrorWindowCreate;
  }
  glfwMakeContextCurrent(window);
  glfwSwapInterval(0);
  if (glewInit() != GLEW_OK) {
    return kErrorGlewInit;
  }
  context->window = window;
  ObjData data;
  error err = obj_data_create(&data);
  if (err != kErrorNil) {
    return err;
  }
  err = obj_data_parse("/mnt/c/Users/niyaz/CLionProjects/ObjViewer_v2.0/obj/cube.obj", &data);
  if (err != kErrorNil) {
    goto cleanup;
  }
  RenderObject* object = (RenderObject*)malloc(sizeof *object);
  if (object == NULL) {
    err = kErrorAllocationFailed;
    goto cleanup;
  }
  err = render_object_create(object, &data);
  if (err != kErrorNil) {
    goto cleanup;
  }
  obj_data_free(&data);
  return render_context_create(object, &context->render_context);
cleanup:
  obj_data_free(&data);
  return err;
}

error window_poll(const WindowContext* context) {
  GLFWwindow* window = context->window;
  while (!glfwWindowShouldClose(window)) {
    update_fps(window);
    render(&context->render_context);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  return kErrorNil;
}

void window_free(const WindowContext* context) {
  render_context_free(&context->render_context);
  glfwTerminate();
}