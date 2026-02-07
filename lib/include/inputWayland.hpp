#pragma once

#include <string>
#include <vector>

#ifdef ENABLE_WAYLAND
#include <pipewire/pipewire.h>
#endif

class InputWayland {
public:
    InputWayland();
    ~InputWayland();

    bool initialize();
    void captureCheck();
    
    // Placeholder for frame data access
    const unsigned char* getData() const;
    int getWidth() const;
    int getHeight() const;

private:
#ifdef ENABLE_WAYLAND
    struct pw_main_loop *loop;
    struct pw_context *context;
    struct pw_core *core;
    struct pw_stream *stream;
#endif
    int width;
    int height;
    std::vector<unsigned char> data;
};
