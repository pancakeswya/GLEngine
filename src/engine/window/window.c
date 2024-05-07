#include "engine/window/window.h"
#include "base/error.h"
#include "base/config.h"
#include "log/log.h"

#include <GLFW/glfw3.h>
#include <stdio.h>

static void update_fps(const Window* window) {
  static double last_update_time = 0;
  static int frames_since_last_update = 0;

  const double now = glfwGetTime();
  frames_since_last_update++;

  if (now - last_update_time > 0.25) {
    const double fps = frames_since_last_update / (now - last_update_time);
    char title_buffer[128];
    sprintf(title_buffer, "(%.1f FPS)", fps);
    glfwSetWindowTitle(*window, title_buffer);
    last_update_time = now;
    frames_since_last_update = 0;
  }
}

error window_create(const WindowConfig* config, Window* window) {
  if (glfwInit() == GLFW_FALSE) {
    LOG_ERR(kErrorGlfwInit);
    return kErrorGlfwInit;
  }
  GLFWmonitor* fullscreen_monitor = NULL;
  GLFWwindow* share = NULL;
  *window = glfwCreateWindow(config->width, config->height, config->name, fullscreen_monitor, share);
  if (*window == NULL) {
    LOG_ERR(kErrorWindowCreate);
    return kErrorWindowCreate;
  }
  glfwMakeContextCurrent(*window);
  glfwSwapInterval(0);
  if (glewInit() != GLEW_OK) {
    LOG_ERR(kErrorGlewInit);
    return kErrorGlewInit;
  }
  return kErrorNil;
}

void window_poll(const Window* window, const RenderFunc render_func, const RenderContext* render_context) {
  while (!glfwWindowShouldClose(*window)) {
    update_fps(window);
    render_func(render_context);
    glfwSwapBuffers(*window);
    glfwPollEvents();
  }
}

void window_terminate(void) {
  glfwTerminate();
}