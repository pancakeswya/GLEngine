#include "base/error.h"
#include "base/vector.h"
#include "engine/window.h"

#ifdef DEBUG
#include "log/log.h"
#endif

int main() {
    WindowContext context;
    error err = window_create(800, 800, "Cube", &context);
    if (err != kErrorNil) {
        goto fail;
    }
    err = window_poll(&context);
    if (err != kErrorNil) {
        goto fail;
    }
    window_detroy();
    return 0;
fail:
#ifdef DEBUG
        LOG_ERR(err);
#endif
 window_detroy(&context);
    return 1;
}
