#include "engine/engine.h"
#include "engine/render/render.h"
#include "engine/window/window.h"
#include "base/error.h"
#include "base/config.h"
#include "log/log.h"

int engine_run(const char* config_path) {
    Config config = {0};
    RenderContext context = {0};
    Window window;
    error err = config_open(config_path, &config);
    if (err != kErrorNil) {
        LOG_ERR(err);
        goto cleanup;
    }
    err = window_create(&config.window, &window);
    if (err != kErrorNil) {
        LOG_ERR(err);
        goto cleanup;
    }
    err = render_init(&config.render, &context);
    if (err != kErrorNil) {
        LOG_ERR(err);
        goto cleanup;
    }
    window_poll(&window, render, &context);
cleanup:
    render_context_free(&context);
    window_terminate();
    config_close(&config);
    return err != kErrorNil;
}