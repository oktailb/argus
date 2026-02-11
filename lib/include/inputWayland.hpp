#pragma once

#include <string>
#include <vector>
#include <cstdint>

// DMABuf frame information - defined outside ifdef for header visibility
struct DMABufFrame {
    int fd;           // File descriptor (owned, will be closed)
    uint32_t width;
    uint32_t height;
    uint32_t stride;
    uint32_t format;  // DRM fourcc format
    uint64_t modifier;
    bool valid;
    
    DMABufFrame() : fd(-1), width(0), height(0), stride(0), 
                    format(0), modifier(0), valid(false) {}
};

#ifdef ENABLE_WAYLAND
#include <pipewire/pipewire.h>
#include <gio/gio.h>
#endif

class InputWayland {
public:
    InputWayland(bool useDMABuf);
    ~InputWayland();

    bool initialize();
    void captureCheck();
    
    // Placeholder for frame data access
    const unsigned char* getData() const;
    int getWidth() const;
    int getHeight() const;
    
    // DMABuf access
    bool hasDMABuf() const;
    const DMABufFrame& getDMABuf() const;

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
    
#ifdef ENABLE_WAYLAND
    // DMABuf frame data
    DMABufFrame current_dmabuf;
    bool has_dmabuf;
    bool use_dmabuf_requested;
    
    // Negotiated parameters
    uint32_t negotiated_format;    // DRM format
    uint64_t negotiated_modifier;  // DRM modifier
#endif
};
