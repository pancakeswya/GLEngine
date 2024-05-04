#include "base/error.h"
#include "engine/window.h"

int main() {
    WindowContext context;
    error err = window_create(800, 800, "Cube", &context);
    if (err == kErrorNil) {
        err = window_poll(&context);
    }
    window_free(&context);
    return err != kErrorNil;
}
