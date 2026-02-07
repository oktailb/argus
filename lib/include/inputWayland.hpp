#pragma once

#include <string>
#include <vector>

#ifdef ENABLE_WAYLAND
#include <pipewire/pipewire.h>
#include <gio/gio.h>
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
    // Friend declarations for PipeWire callbacks
    friend void on_process(void *userdata);
    friend void on_param_changed(void *userdata, uint32_t id, const struct spa_pod *param);
    
    struct pw_main_loop *loop;
    struct pw_context *context;
    struct pw_core *core;
    struct pw_stream *stream;
    
    // xdg-desktop-portal D-Bus
    GDBusProxy *portal_proxy;
    char *session_handle;
    uint32_t pipewire_node_id;
    int pipewire_fd;
    
    // Internal methods
    bool requestScreenCast();
    bool createStream();
    static void onStartResponse(GDBusProxy *proxy, GAsyncResult *res, gpointer user_data);
#endif
    int width;
    int height;
    std::vector<unsigned char> data;
};
