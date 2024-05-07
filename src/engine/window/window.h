#ifndef ENGINE_WINDOW_WINDOW_H_
#define ENGINE_WINDOW_WINDOW_H_

#include "base/error.h"
#include "engine/render/context.h"

typedef struct GLFWwindow* Window;
typedef struct WindowConfig WindowConfig;
typedef void (*RenderFunc)(const RenderContext*);

error window_create(const WindowConfig* config, Window* window);
void window_poll(const Window* window, const RenderFunc render_func, const RenderContext* render_context);
void window_terminate(void);

#endif // ENGINE_WINDOW_WINDOW_H_