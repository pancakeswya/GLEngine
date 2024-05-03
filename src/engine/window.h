#ifndef WINDOW_H_
#define WINDOW_H_

#include "base/error.h"
#include "engine/render.h"

#include <GLFW/glfw3.h>

typedef struct {
    RenderContext render_context;
    GLFWwindow* window;
} WindowContext;

error window_create(int width, int height, const char* title, WindowContext* context);
error window_poll(const WindowContext* context);
void window_detroy();

#endif // WINDOW_H_