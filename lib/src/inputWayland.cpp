#include "inputWayland.hpp"
#include <iostream>

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

InputWayland::InputWayland() : width(0), height(0) {
#ifdef ENABLE_WAYLAND
    loop = nullptr;
    context = nullptr;
    core = nullptr;
    stream = nullptr;
#endif
}

InputWayland::~InputWayland() {
#ifdef ENABLE_WAYLAND
    if (stream) {
        pw_stream_destroy(stream);
        stream = nullptr;
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
    /* TODO: Request screencast via xdg-desktop-portal (D-Bus ScreenCast),
     * then create pw_stream with the received fd. Until then, refuse Wayland
     * so the app falls back to X11. */
    std::cerr << "Argus Wayland: PipeWire context ready; screencast via portal not yet implemented (use method=x11)" << std::endl;
    pw_core_disconnect(core);
    core = nullptr;
    pw_context_destroy(context);
    context = nullptr;
    pw_main_loop_destroy(loop);
    loop = nullptr;
    return false;
#else
    std::cerr << "Argus: Wayland support not compiled in (no libpipewire-0.3)" << std::endl;
    return false;
#endif
}

void InputWayland::captureCheck() {
#ifdef ENABLE_WAYLAND
    if (loop)
    pw_main_loop_events(loop);
#endif
}

const unsigned char* InputWayland::getData() const {
    return data.data();
}

int InputWayland::getWidth() const {
    return width;
}

int InputWayland::getHeight() const {
    return height;
}
