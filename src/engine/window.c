#include "engine/window.h"
#include "engine/render.h"
#include "base/error.h"

#ifdef DEBUG
#include "log/log.h"
#endif

#include <stdio.h>

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
  const RenderObject object = {
    .vertices = (GLfloat[]){
      // Front face
      0.5,  0.5,  0.5,
     -0.5,  0.5,  0.5,
     -0.5, -0.5,  0.5,
      0.5, -0.5,  0.5,

     // Back face
      0.5,  0.5, -0.5,
     -0.5,  0.5, -0.5,
     -0.5, -0.5, -0.5,
      0.5, -0.5, -0.5,
   },
    .indices = (GLuint[]){
      0, 1, 2,
     2, 3, 0,

     // Right
     0, 3, 7,
     7, 4, 0,

     // Bottom
     2, 6, 7,
     7, 3, 2,

     // Left
     1, 5, 6,
     6, 2, 1,

     // Back
     4, 7, 6,
     6, 5, 4,

     // Top
     5, 1, 0,
     0, 4, 5,
    },
    .colors = (GLfloat[]){
      1.0, 0.4, 0.6,
      1.0, 0.9, 0.2,
      0.7, 0.3, 0.8,
      0.5, 0.3, 1.0,

      0.2, 0.6, 1.0,
      0.6, 1.0, 0.4,
      0.6, 0.8, 0.8,
      0.4, 0.8, 0.8,
    },
    .verticies_size = 24 * sizeof(GLfloat),
    .indices_size = 36 * sizeof(GLuint),
    .colors_size = 24 * sizeof(GLfloat)
  };
  return render_context_create(&object, &context->render_context);
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

void window_detroy() {
  glfwTerminate();
}