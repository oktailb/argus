#include "inputWayland.hpp"
#include <iostream>
#include <chrono>
#ifdef ENABLE_WAYLAND
#include <gio/gio.h>
#include <spa/param/video/format-utils.h>
#include <spa/param/video/type-info.h>
#include <spa/utils/result.h>
#include <spa/pod/builder.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <sys/mman.h>
#include <cerrno>
#endif

/*
 * Wayland capture (PipeWire + xdg-desktop-portal ScreenCast)
 *
 * Pour terminer l’implémentation :
 * 1. Appeler org.freedesktop.portal.ScreenCast via D-Bus (libportal ou GDBus)
 *    pour obtenir un fd de flux PipeWire.
 * 2. Créer un pw_stream avec ce fd, négocier les paramètres (format BGRA, résolution).
 * 3. Dans on_process(), copier les buffers reçus dans this->data (width*height*4).
 * 4. Appeler pw_main_loop_run() dans un thread ou de manière non bloquante (pw_loop_iterate).
 * 5. Mettre à jour width/height dès la première frame reçue.
 *
 * Référence : xdg-desktop-portal-wlr (pipewire_screencast.c)
 */

InputWayland::InputWayland(bool useDMABuf) : width(0), height(0) {
#ifdef ENABLE_WAYLAND
    use_dmabuf_requested = useDMABuf;
    loop = nullptr;
    context = nullptr;
    core = nullptr;
    stream = nullptr;
    portal_proxy = nullptr;
    session_handle = nullptr;
    pipewire_node_id = 0;
    pipewire_fd = -1;
    has_dmabuf = false;
    negotiated_format = 0;
    negotiated_modifier = 0;
#endif
}

InputWayland::~InputWayland() {
#ifdef ENABLE_WAYLAND
    // Clean up DMABuf FD if owned
    if (current_dmabuf.valid && current_dmabuf.fd >= 0) {
        close(current_dmabuf.fd);
        current_dmabuf.fd = -1;
        current_dmabuf.valid = false;
    }
    
    if (stream) {
        pw_stream_destroy(stream);
        stream = nullptr;
    }
    if (pipewire_fd >= 0) {
        close(pipewire_fd);
        pipewire_fd = -1;
    }
    if (session_handle) {
        g_free(session_handle);
        session_handle = nullptr;
    }
    if (portal_proxy) {
        g_object_unref(portal_proxy);
        portal_proxy = nullptr;
    }
    if (loop) {
        pw_main_loop_destroy(loop);
        loop = nullptr;
    }
    if (context) {
        pw_context_destroy(context);
        context = nullptr;
    }
#endif
}

bool InputWayland::initialize() {
#ifdef ENABLE_WAYLAND
    pw_init(nullptr, nullptr);
    loop = pw_main_loop_new(nullptr);
    if (!loop) {
        std::cerr << "Argus Wayland: failed to create PipeWire main loop" << std::endl;
        return false;
    }
    context = pw_context_new(pw_main_loop_get_loop(loop), nullptr, 0);
    if (!context) {
        std::cerr << "Argus Wayland: failed to create PipeWire context" << std::endl;
        return false;
    }
    core = pw_context_connect(context, nullptr, 0);
    if (!core) {
        std::cerr << "Argus Wayland: failed to connect PipeWire context" << std::endl;
        return false;
    }
    
    // Request screencast via xdg-desktop-portal
    if (!requestScreenCast()) {
        std::cerr << "Argus Wayland: failed to request screencast" << std::endl;
        pw_core_disconnect(core);
        core = nullptr;
        pw_context_destroy(context);
        context = nullptr;
        pw_main_loop_destroy(loop);
        loop = nullptr;
        return false;
    }
    
    std::cerr << "Argus Wayland: ScreenCast initialized successfully" << std::endl;
    return true;
#else
    std::cerr << "Argus: Wayland support not compiled in (no libpipewire-0.3)" << std::endl;
    return false;
#endif
}

void InputWayland::captureCheck() {
#ifdef ENABLE_WAYLAND
    if (loop)
        pw_loop_iterate(pw_main_loop_get_loop(loop), 0);  // Non-blocking
#endif
}

#ifdef ENABLE_WAYLAND
// Portal D-Bus implementation
bool InputWayland::requestScreenCast() {
    GError *error = nullptr;
    
    // Connect to xdg-desktop-portal
    portal_proxy = g_dbus_proxy_new_for_bus_sync(
        G_BUS_TYPE_SESSION,
        G_DBUS_PROXY_FLAGS_NONE,
        nullptr,
        "org.freedesktop.portal.Desktop",
        "/org/freedesktop/portal/desktop",
        "org.freedesktop.portal.ScreenCast",
        nullptr,
        &error);
    
    if (error) {
        std::cerr << "Failed to connect to portal: " << error->message << std::endl;
        g_error_free(error);
        return false;
    }
    
    // Step 1: CreateSession
    // Generate proper session token based on D-Bus sender name
    const char *sender = g_dbus_connection_get_unique_name(
        g_dbus_proxy_get_connection(portal_proxy));
    
    // Convert sender name (e.g., ":1.123") to valid handle token (e.g., "t1_123")
    std::string sender_str(sender);
    std::string token = "argus_";
    for (char c : sender_str) {
        if (c == ':') token += 't';
        else if (c == '.') token += '_';
        else token += c;
    }
    
    GVariantBuilder options_builder;
    g_variant_builder_init(&options_builder, G_VARIANT_TYPE("a{sv}"));
    g_variant_builder_add(&options_builder, "{sv}", "handle_token", 
                          g_variant_new_string(token.c_str()));
    g_variant_builder_add(&options_builder, "{sv}", "session_handle_token", 
                          g_variant_new_string("argus_session"));
    
    GVariant *result = g_dbus_proxy_call_sync(
        portal_proxy,
        "CreateSession",
        g_variant_new("(a{sv})", &options_builder),
        G_DBUS_CALL_FLAGS_NONE,
        -1,
        nullptr,
        &error);
    
    if (error) {
        std::cerr << "CreateSession failed: " << error->message << std::endl;
        g_error_free(error);
        return false;
    }
    
    // CreateSession returns a request path, we need to get session from Response
    const char *request_path;
    g_variant_get(result, "(o)", &request_path);
    g_variant_unref(result);
    
    std::cerr << "Request created: " << request_path << std::endl;
    
    // Wait for Response signal (synchronous approach via blocking call)
    // The session handle is constructed from the response
    GDBusConnection *connection = g_dbus_proxy_get_connection(portal_proxy);
    GDBusProxy *request_proxy = g_dbus_proxy_new_sync(
        connection,
        G_DBUS_PROXY_FLAGS_NONE,
        nullptr,
        "org.freedesktop.portal.Desktop",
        request_path,
        "org.freedesktop.portal.Request",
        nullptr,
        &error);
    
    if (error) {
        std::cerr << "Failed to create request proxy: " << error->message << std::endl;
        g_error_free(error);
        return false;
    }
    
    // Use simpler approach: directly construct session path
    // Session handle format: /org/freedesktop/portal/desktop/session/{sender}/{token}
    std::string session_token = "argus_session";
    std::string session_path_str = "/org/freedesktop/portal/desktop/session/";
    for (size_t i = 1; i < sender_str.length(); ++i) {  // Skip leading ':'
        char c = sender_str[i];
        if (c == '.') session_path_str += '_';
        else session_path_str += c;
    }
    session_path_str += "/" + session_token;
    
    session_handle = g_strdup(session_path_str.c_str());
    g_object_unref(request_proxy);
    
    std::cerr << "Session handle: " << session_handle << std::endl;
    
    // Step 2: SelectSources
    g_variant_builder_init(&options_builder, G_VARIANT_TYPE("a{sv}"));
    g_variant_builder_add(&options_builder, "{sv}", "types", g_variant_new_uint32(1 | 2));  // Monitor | Window
    g_variant_builder_add(&options_builder, "{sv}", "multiple", g_variant_new_boolean(FALSE));
    
    result = g_dbus_proxy_call_sync(
        portal_proxy,
        "SelectSources",
        g_variant_new("(oa{sv})", session_handle, &options_builder),
        G_DBUS_CALL_FLAGS_NONE,
        -1,
        nullptr,
        &error);
    
    if (error) {
        std::cerr << "SelectSources failed: " << error->message << std::endl;
        g_error_free(error);
        return false;
    }
    
    // SelectSources returns a request path - we need to wait for Response
    const char *select_request_path;
    g_variant_get(result, "(o)", &select_request_path);
    std::cerr << "SelectSources request: " << select_request_path << std::endl;
    g_variant_unref(result);
    
    // Wait for user to select sources (blocking with timeout)
    std::cerr << "Waiting for user to select sources..." << std::endl;
    
    // Helper struct for response handling
    struct SelectSourcesData {
        bool received;
        uint32_t code;
        GMainLoop *loop;
    };
    
    SelectSourcesData select_data = { false, 1, g_main_loop_new(nullptr, FALSE) };
    
    // Static callback for Response signal
    static auto response_callback = [](GDBusConnection*, const gchar* sender,
                                       const gchar* object_path, const gchar* interface,
                                       const gchar* signal_name, GVariant* params,
                                       gpointer user_data) {
        SelectSourcesData *data = (SelectSourcesData*)user_data;
        
        uint32_t code;
        GVariant *results;
        g_variant_get(params, "(u@a{sv})", &code, &results);
        
        data->received = true;
        data->code = code;
        g_variant_unref(results);
        
        g_main_loop_quit(data->loop);
    };
    
    // Subscribe to Response signal on the request path
    guint subscription_id = g_dbus_connection_signal_subscribe(
        connection,
        "org.freedesktop.portal.Desktop",
        "org.freedesktop.portal.Request",
        "Response",
        select_request_path,
        nullptr,
        G_DBUS_SIGNAL_FLAGS_NONE,
        +response_callback,  // Convert lambda to function pointer
        &select_data,
        nullptr);
    
    // Run loop with 30 second timeout
    guint timeout_id = g_timeout_add_seconds(30, +[](gpointer data) -> gboolean {
        g_main_loop_quit((GMainLoop*)data);
        return G_SOURCE_REMOVE;
    }, select_data.loop);
    
    g_main_loop_run(select_data.loop);
    
    g_source_remove(timeout_id);
    g_dbus_connection_signal_unsubscribe(connection, subscription_id);
    g_main_loop_unref(select_data.loop);
    
    if (!select_data.received || select_data.code != 0) {
        std::cerr << "SelectSources cancelled or timed out (code: " << select_data.code << ")" << std::endl;
        return false;
    }
    
    std::cerr << "Sources selected successfully" << std::endl;
    
    // Step 3: Start
    g_variant_builder_init(&options_builder, G_VARIANT_TYPE("a{sv}"));
    g_variant_builder_add(&options_builder, "{sv}", "handle_token", g_variant_new_string("argus_start"));
    
    result = g_dbus_proxy_call_sync(
        portal_proxy,
        "Start",
        g_variant_new("(osa{sv})", session_handle, "", &options_builder),
        G_DBUS_CALL_FLAGS_NONE,
        -1,
        nullptr,
        &error);
    
    if (error) {
        std::cerr << "Start failed: " << error->message << std::endl;
        g_error_free(error);
        return false;
    }
    
    // Start also returns a request path - wait for Response
    const char *start_request_path;
    g_variant_get(result, "(o)", &start_request_path);
    std::cerr << "Start request: " << start_request_path << std::endl;
    g_variant_unref(result);
    
    std::cerr << "Waiting for Start response..." << std::endl;
    
    // Reuse the same pattern for Start response
    struct StartResponseData {
        bool received;
        uint32_t code;
        GVariant *results;
        GMainLoop *loop;
    };
    
    StartResponseData start_resp = { false, 1, nullptr, g_main_loop_new(nullptr, FALSE) };
    
    static auto start_response_callback = [](GDBusConnection*, const gchar*,
                                             const gchar*, const gchar*,
                                             const gchar*, GVariant* params,
                                             gpointer user_data) {
        StartResponseData *data = (StartResponseData*)user_data;
        
        uint32_t code;
        GVariant *results;
        g_variant_get(params, "(u@a{sv})", &code, &results);
        
        data->received = true;
        data->code = code;
        data->results = results;  // Keep reference
        
        g_main_loop_quit(data->loop);
    };
    
    guint start_sub_id = g_dbus_connection_signal_subscribe(
        connection,
        "org.freedesktop.portal.Desktop",
        "org.freedesktop.portal.Request",
        "Response",
        start_request_path,
        nullptr,
        G_DBUS_SIGNAL_FLAGS_NONE,
        +start_response_callback,
        &start_resp,
        nullptr);
    
    guint start_timeout_id = g_timeout_add_seconds(30, +[](gpointer data) -> gboolean {
        g_main_loop_quit((GMainLoop*)data);
        return G_SOURCE_REMOVE;
    }, start_resp.loop);
    
    g_main_loop_run(start_resp.loop);
    
    g_source_remove(start_timeout_id);
    g_dbus_connection_signal_unsubscribe(connection, start_sub_id);
    g_main_loop_unref(start_resp.loop);
    
    if (!start_resp.received || start_resp.code != 0) {
        std::cerr << "Start cancelled or denied (code: " << start_resp.code << ")" << std::endl;
        if (start_resp.results) g_variant_unref(start_resp.results);
        return false;
    }
    
    // Parse Start response to get PipeWire streams
    GVariant *streams_variant = g_variant_lookup_value(start_resp.results, "streams", G_VARIANT_TYPE("a(ua{sv})"));
    if (!streams_variant) {
        std::cerr << "No streams in Start response" << std::endl;
        g_variant_unref(start_resp.results);
        return false;
    }
    
    GVariantIter iter;
    g_variant_iter_init(&iter, streams_variant);
    GVariant *stream_properties;
    if (g_variant_iter_next(&iter, "(u@a{sv})", &pipewire_node_id, &stream_properties)) {
        std::cerr << "PipeWire node ID: " << pipewire_node_id << std::endl;
        g_variant_unref(stream_properties);
    }
    g_variant_unref(streams_variant);
    g_variant_unref(start_resp.results);
    
    // Get PipeWire FD via OpenPipeWireRemote
    GUnixFDList *fd_list = nullptr;
    result = g_dbus_proxy_call_with_unix_fd_list_sync(
        portal_proxy,
        "OpenPipeWireRemote",
        g_variant_new("(oa{sv})", session_handle, nullptr),
        G_DBUS_CALL_FLAGS_NONE,
        -1,
        nullptr,
        &fd_list,
        nullptr,
        &error);
    
    if (error) {
        std::cerr << "OpenPipeWireRemote failed: " << error->message << std::endl;
        g_error_free(error);
        return false;
    }
    
    int32_t fd_index;
    g_variant_get(result, "(h)", &fd_index);
    pipewire_fd = g_unix_fd_list_get(fd_list, fd_index, &error);
    g_object_unref(fd_list);
    g_variant_unref(result);
    
    if (pipewire_fd < 0) {
        std::cerr << "Failed to get PipeWire FD" << std::endl;
        return false;
    }
    
    std::cerr << "PipeWire FD: " << pipewire_fd << std::endl;
    
    // Create PipeWire stream
    if (!createStream()) {
        std::cerr << "Failed to create PipeWire stream" << std::endl;
        return false;
    }
    
    // Wait for stream format to be negotiated (timeout 5 seconds)
    std::cerr << "Waiting for stream format..." << std::endl;
    auto start_time = std::chrono::steady_clock::now();
    while (width == 0 || height == 0) {
        pw_loop_iterate(pw_main_loop_get_loop(loop), 10);  // 10ms timeout
        
        auto elapsed = std::chrono::steady_clock::now() - start_time;
        if (std::chrono::duration_cast<std::chrono::seconds>(elapsed).count() > 5) {
            std::cerr << "Timeout waiting for stream format" << std::endl;
            return false;
        }
    }
    
    std::cerr << "Argus Wayland: ScreenCast initialized successfully (" 
              << width << "x" << height << ")" << std::endl;
    return true;
}

// PipeWire stream events
void on_process(void *userdata) {
    InputWayland *self = (InputWayland*)userdata;
    struct pw_buffer *buf = pw_stream_dequeue_buffer(self->stream);
    if (!buf) return;
    
    struct spa_buffer *spa_buf = buf->buffer;
    struct spa_data *d = &spa_buf->datas[0];
    
    // Debug: log data type every 120 frames (~2s at 60fps)
    static int frame_count = 0;
    bool should_log = (frame_count++ % 120 == 0);
    
    if (should_log) {
        std::cerr << "[Frame " << frame_count << "] Data type: " << d->type 
                  << " (MemPtr=1, MemFd=2, DmaBuf=3), FD=" << d->fd << std::endl;
    }
    
    if (d->type == SPA_DATA_MemPtr && d->data) {
        // CPU copy fallback - direct memory pointer
        if (should_log) {
            std::cerr << "  -> Using MemPtr (CPU copy)" << std::endl;
        }
        size_t size = d->chunk->size;
        if (self->data.size() != size) {
            self->data.resize(size);
        }
        memcpy(self->data.data(), d->data, size);
        self->has_dmabuf = false;  // Not using DMABuf
        
    } else if (d->type == SPA_DATA_MemFd) {
        // Memory file descriptor - need to map it
        if (should_log) {
            std::cerr << "  -> Using MemFd (mmap copy)" << std::endl;
        }
        size_t size = d->chunk->size;
        size_t offset = d->chunk->offset;
        
        if (d->fd >= 0 && size > 0) {
            // Map the file descriptor
            void *mapped = mmap(nullptr, d->maxsize, PROT_READ, MAP_SHARED, d->fd, 0);
            if (mapped != MAP_FAILED) {
                if (self->data.size() != size) {
                    self->data.resize(size);
                }
                memcpy(self->data.data(), (uint8_t*)mapped + offset, size);
                munmap(mapped, d->maxsize);
            } else {
                static bool logged = false;
                if (!logged) {
                    std::cerr << "Failed to mmap MemFd: " << strerror(errno) << std::endl;
                    logged = true;
                }
            }
        }
        self->has_dmabuf = false;  // Not using DMABuf
        
    } else if (d->type == SPA_DATA_DmaBuf) {
        // DMABuf path - extract FD and metadata
        if (should_log) {
            std::cerr << "  -> DMABuf detected! FD=" << d->fd << std::endl;
        }
        
        if (d->fd >= 0) {
            // Clean up previous DMABuf if any
            if (self->current_dmabuf.valid && self->current_dmabuf.fd >= 0) {
                close(self->current_dmabuf.fd);
            }
            
            // Duplicate the FD so we own it (PipeWire will close the original)
            self->current_dmabuf.fd = dup(d->fd);
            self->current_dmabuf.width = self->width;
            self->current_dmabuf.height = self->height;
            
            // Extract stride from chunk or calculate default
            if (d->chunk->stride > 0) {
                self->current_dmabuf.stride = d->chunk->stride;
            } else {
                // Fallback: assume BGRA (4 bytes per pixel)
                self->current_dmabuf.stride = self->width * 4;
            }
            
            // Format and modifier extraction
            self->current_dmabuf.format = self->negotiated_format;
            self->current_dmabuf.modifier = self->negotiated_modifier;
            
            // If format was not negotiated (should not happen if on_param_changed called), fallback
            if (self->current_dmabuf.format == 0) {
                 self->current_dmabuf.format = 0x34325241; // DRM_FORMAT_ARGB8888
            }
            
            self->current_dmabuf.valid = true;
            self->has_dmabuf = true;
            
            if (should_log) {
                std::cerr << "  -> DMABuf configured: FD=" << self->current_dmabuf.fd 
                          << ", " << self->width << "x" << self->height 
                          << ", stride=" << self->current_dmabuf.stride 
                          << ", has_dmabuf=true" << std::endl;
            }
        } else {
            std::cerr << "  -> DMABuf has invalid FD!" << std::endl;
            self->has_dmabuf = false;
        }
        
    } else {
        if (should_log) {
            std::cerr << "  -> Unknown data type: " << d->type << std::endl;
        }
        self->has_dmabuf = false;
    }
    
    pw_stream_queue_buffer(self->stream, buf);
}

void on_param_changed(void *userdata, uint32_t id, const struct spa_pod *param)  {
    InputWayland *self = (InputWayland*)userdata;
    
    if (param == nullptr || id != SPA_PARAM_Format) return;
    
    struct spa_video_info_raw info;
    if (spa_format_video_raw_parse(param, &info) < 0) return;
    
    self->width = info.size.width;
    self->height = info.size.height;
    size_t stride = info.size.width * 4;  // BGRA
    self->data.resize(stride * info.size.height);
    
    std::cerr << "Stream format: " << info.size.width << "x" << info.size.height << std::endl;

    // Parse modifier and format
    const struct spa_pod_prop *prop;
    struct spa_pod *prop_value;

    // Reset default
    self->negotiated_modifier = 0; // DRM_FORMAT_MOD_LINEAR
    self->negotiated_format = 0;

    // Find modifier in properties
    prop = spa_pod_find_prop(param, nullptr, SPA_FORMAT_VIDEO_modifier);
    if (prop) {
        if (spa_pod_is_long(&prop->value)) {
            spa_pod_get_long(&prop->value, (int64_t*)&self->negotiated_modifier);
            std::cerr << "  -> Negotiated modifier: " << self->negotiated_modifier << std::endl;
        }
    }

    // Map SPA format to DRM format
    // Simple mapping for common formats
    if (info.format == SPA_VIDEO_FORMAT_BGRA) {
        self->negotiated_format = 0x34325241; // DRM_FORMAT_ARGB8888 (little endian)
    } else if (info.format == SPA_VIDEO_FORMAT_RGBA) {
        self->negotiated_format = 0x34324142; // DRM_FORMAT_ABGR8888 (little endian)
    } else if (info.format == SPA_VIDEO_FORMAT_RGBx) {
        self->negotiated_format = 0x34324258; // DRM_FORMAT_XBGR8888
    } else if (info.format == SPA_VIDEO_FORMAT_BGRx) {
        self->negotiated_format = 0x34325258; // DRM_FORMAT_XRGB8888
    } else {
        std::cerr << "  -> Warning: Unknown SPA format " << info.format << ", defaulting to ARGB8888" << std::endl;
        self->negotiated_format = 0x34325241; 
    }
}

bool InputWayland::createStream() {
    static const struct pw_stream_events stream_events = {
        .version = PW_VERSION_STREAM_EVENTS,
        .param_changed = on_param_changed,
        .process = on_process,
    };
    
    stream = pw_stream_new_simple(
        pw_main_loop_get_loop(loop),
        "argus-capture",
        pw_properties_new(
            PW_KEY_MEDIA_TYPE, "Video",
            PW_KEY_MEDIA_CATEGORY, "Capture",
            PW_KEY_MEDIA_ROLE, "Screen",
            nullptr
        ),
        &stream_events,
        this
    );
    
    if (!stream) {
        std::cerr << "Failed to create pw_stream" << std::endl;
        return false;
    }
    
    // Build format parameters and buffer data type
    uint8_t buffer[2048];  // Increased size for multiple params
    struct spa_pod_builder b = SPA_POD_BUILDER_INIT(buffer, sizeof(buffer));
    const struct spa_pod *params[2];  // Format + Buffers
    
    std::cerr << "Requesting DMABuf data type (with MemFd fallback)" << std::endl;

    if (use_dmabuf_requested) {
         // Param 0: EnumFormat (video format) WITH modifiers
        params[0] = (const struct spa_pod*)spa_pod_builder_add_object(&b,
            SPA_TYPE_OBJECT_Format, SPA_PARAM_EnumFormat,
            SPA_FORMAT_mediaType, SPA_POD_Id(SPA_MEDIA_TYPE_video),
            SPA_FORMAT_mediaSubtype, SPA_POD_Id(SPA_MEDIA_SUBTYPE_raw),
            SPA_FORMAT_VIDEO_format, SPA_POD_CHOICE_ENUM_Id(3,
                SPA_VIDEO_FORMAT_BGRA,
                SPA_VIDEO_FORMAT_BGRA,
                SPA_VIDEO_FORMAT_RGB),
            SPA_FORMAT_VIDEO_modifier, SPA_POD_CHOICE_FLAGS_Long(0)
        );

        // Param 1: Buffers - request DMABuf data type
        params[1] = (const struct spa_pod*)spa_pod_builder_add_object(&b,
            SPA_TYPE_OBJECT_ParamBuffers, SPA_PARAM_Buffers,
            SPA_PARAM_BUFFERS_dataType, SPA_POD_CHOICE_FLAGS_Int((1 << SPA_DATA_DmaBuf) | (1 << SPA_DATA_MemFd))
        );
         std::cerr << "  -> DMABuf enabled in config" << std::endl;
    } else {
        // Param 0: EnumFormat (video format) WITHOUT modifiers (implicit modifier: Linear)
         params[0] = (const struct spa_pod*)spa_pod_builder_add_object(&b,
            SPA_TYPE_OBJECT_Format, SPA_PARAM_EnumFormat,
            SPA_FORMAT_mediaType, SPA_POD_Id(SPA_MEDIA_TYPE_video),
            SPA_FORMAT_mediaSubtype, SPA_POD_Id(SPA_MEDIA_SUBTYPE_raw),
            SPA_FORMAT_VIDEO_format, SPA_POD_CHOICE_ENUM_Id(3,
                SPA_VIDEO_FORMAT_BGRA,
                SPA_VIDEO_FORMAT_BGRA,
                SPA_VIDEO_FORMAT_RGB)
        );

        // Param 1: Buffers - request MemFd/MemPtr only
        params[1] = (const struct spa_pod*)spa_pod_builder_add_object(&b,
            SPA_TYPE_OBJECT_ParamBuffers, SPA_PARAM_Buffers,
            SPA_PARAM_BUFFERS_dataType, SPA_POD_CHOICE_FLAGS_Int((1 << SPA_DATA_MemFd) | (1 << SPA_DATA_MemPtr))
        );
        std::cerr << "  -> DMABuf DISABLED in config (MemFd/MemPtr only)" << std::endl;
    }
    
    // Connect to the PipeWire node WITHOUT MAP_BUFFERS flag
    int res = pw_stream_connect(stream,
        PW_DIRECTION_INPUT,
        pipewire_node_id,
        (enum pw_stream_flags)(
            PW_STREAM_FLAG_AUTOCONNECT
            // Removed PW_STREAM_FLAG_MAP_BUFFERS to allow DMABuf
        ),
        params, 2);  // Now passing 2 params instead of 1
    
    if (res < 0) {
        std::cerr << "Failed to connect stream: " << spa_strerror(res) << std::endl;
        return false;
    }
    
    std::cerr << "PipeWire stream connected to node " << pipewire_node_id << std::endl;
    return true;
}
#endif

const unsigned char* InputWayland::getData() const {
    return data.data();
}

int InputWayland::getWidth() const {
    return width;
}

int InputWayland::getHeight() const {
    return height;
}

bool InputWayland::hasDMABuf() const {
#ifdef ENABLE_WAYLAND
    return has_dmabuf && current_dmabuf.valid;
#else
    return false;
#endif
}

const DMABufFrame& InputWayland::getDMABuf() const {
#ifdef ENABLE_WAYLAND
    return current_dmabuf;
#else
    static DMABufFrame dummy;
    return dummy;
#endif
}
