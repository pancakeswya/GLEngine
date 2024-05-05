#ifndef ENGINE_RENDER_H_
#define ENGINE_RENDER_H_

#include "base/error.h"
#include "engine/render/context.h"

error render_init(RenderContext* context);
void render(const RenderContext* context);

#endif // ENGINE_RENDER_H_