#ifndef ENGINE_RENDER_RENDER_H_
#define ENGINE_RENDER_RENDER_H_

#include "base/error.h"
#include "engine/render/context.h"

typedef struct RenderConfig RenderConfig;

error render_init(const RenderConfig* config, RenderContext* context);
void render(const RenderContext* context);

#endif // ENGINE_RENDER_RENDER_H_