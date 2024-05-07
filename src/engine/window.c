#include "engine/window.h"
#include "engine/render/render.h"
#include "base/error.h"
#include "log/log.h"

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
    LOG_ERR(kErrorGlfwInit);
    return kErrorGlfwInit;
  }
  GLFWmonitor* fullscreen_monitor = NULL;
  GLFWwindow* share = NULL;
  GLFWwindow* window = glfwCreateWindow(width, height, title, fullscreen_monitor, share);
  if (window == NULL) {
    LOG_ERR(kErrorWindowCreate);
    return kErrorWindowCreate;
  }
  glfwMakeContextCurrent(window);
  glfwSwapInterval(0);
  if (glewInit() != GLEW_OK) {
    LOG_ERR(kErrorGlewInit);
    return kErrorGlewInit;
  }
  context->window = window;
  return render_init(&context->render_context);
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
  glfwTerminate();
  if (context == NULL) {
    return;
  }
  render_context_free(&context->render_context);
}