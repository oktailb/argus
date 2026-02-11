#include "argusConfig.h"
#include "configuration.h"
#include <cmath>
#include <sstream>
#include <algorithm>
#include <cstdlib>

namespace argus {

namespace {

template<typename T>
T getOrDefault(const std::map<std::string, std::string>& m, const std::string& key, T defaultVal);

template<>
int getOrDefault<int>(const std::map<std::string, std::string>& m, const std::string& key, int defaultVal) {
    auto it = m.find(key);
    if (it == m.end() || it->second.empty()) return defaultVal;
    try {
        return std::stoi(it->second);
    } catch (...) {
        return defaultVal;
    }
}

template<>
double getOrDefault<double>(const std::map<std::string, std::string>& m, const std::string& key, double defaultVal) {
    auto it = m.find(key);
    if (it == m.end() || it->second.empty()) return defaultVal;
    try {
        return std::stod(it->second);
    } catch (...) {
        return defaultVal;
    }
}

template<>
float getOrDefault<float>(const std::map<std::string, std::string>& m, const std::string& key, float defaultVal) {
    return static_cast<float>(getOrDefault<double>(m, key, static_cast<double>(defaultVal)));
}

template<>
std::string getOrDefault<std::string>(const std::map<std::string, std::string>& m, const std::string& key, std::string defaultVal) {
    auto it = m.find(key);
    if (it == m.end()) return defaultVal;
    return it->second;
}

template<>
bool getOrDefault<bool>(const std::map<std::string, std::string>& m, const std::string& key, bool defaultVal) {
    auto it = m.find(key);
    if (it == m.end() || it->second.empty()) return defaultVal;
    std::string v = it->second;
    std::transform(v.begin(), v.end(), v.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return (v == "1" || v == "true" || v == "yes");
}

void clamp(int& v, int lo, int hi) {
    if (v < lo) v = lo;
    if (v > hi) v = hi;
}
void clamp(double& v, double lo, double hi) {
    if (v < lo) v = lo;
    if (v > hi) v = hi;
}
void clamp(float& v, float lo, float hi) {
    if (v < lo) v = lo;
    if (v > hi) v = hi;
}

} // anonymous

ConfigLoadResult loadConfig(const std::string& filename, ArgusConfig& out) {
    ConfigLoadResult result;
    result.ok = true;
    std::map<std::string, std::string> m = readConfiguration(filename);

    if (m.empty()) {
        result.ok = false;
        result.error = "Impossible d'ouvrir ou lire le fichier: " + filename;
        return result;
    }
    
    // --- General ---
    out.general.child        = getOrDefault(m, "General/Child", out.general.child);
    out.general.prefix       = getOrDefault(m, "General/Prefix", out.general.prefix);
    out.general.title        = getOrDefault(m, "General/title", out.general.title);
    out.general.captureMethod= getOrDefault(m, "General/method", out.general.captureMethod);
    out.general.pillowRec    = getOrDefault(m, "General/PillowRec", out.general.pillowRec);
    out.general.quadRec      = getOrDefault(m, "General/QuadRec", out.general.quadRec);
    out.general.smoothLen    = getOrDefault(m, "General/SmoothLen", out.general.smoothLen);
    out.general.fps          = getOrDefault(m, "General/fps", out.general.fps);
    out.general.videoSync    = getOrDefault(m, "General/videoSync", out.general.videoSync);
    out.general.stats        = getOrDefault(m, "General/stats", out.general.stats);
    out.general.virtualDesktop = getOrDefault(m, "General/virtualDesktop", out.general.virtualDesktop);
    out.general.webServerInterface = getOrDefault(m, "General/webServerInterface", out.general.webServerInterface);
    out.general.webServerPort = getOrDefault(m, "General/webServerPort", out.general.webServerPort);

    clamp(out.general.pillowRec, 1, 15);
    clamp(out.general.quadRec, 1, 15);
    clamp(out.general.smoothLen, 0.0, 1.0);
    if (out.general.fps < 1) out.general.fps = 1;
    if (out.general.fps > 240) out.general.fps = 240;

    // --- Geometry ---
    out.geometry.Nx  = getOrDefault(m, "Geometry/Nx",  out.geometry.Nx);
    out.geometry.Ny  = getOrDefault(m, "Geometry/Ny",  out.geometry.Ny);
    out.geometry.Sx  = getOrDefault(m, "Geometry/Sx",  out.geometry.Sx);
    out.geometry.Sy  = getOrDefault(m, "Geometry/Sy",  out.geometry.Sy);
    out.geometry.Ex  = getOrDefault(m, "Geometry/Ex",  out.geometry.Ex);
    out.geometry.Ey  = getOrDefault(m, "Geometry/Ey",  out.geometry.Ey);
    out.geometry.Wx  = getOrDefault(m, "Geometry/Wx",  out.geometry.Wx);
    out.geometry.Wy  = getOrDefault(m, "Geometry/Wy",  out.geometry.Wy);
    out.geometry.Cx  = getOrDefault(m, "Geometry/Cx",  out.geometry.Cx);
    out.geometry.Cy  = getOrDefault(m, "Geometry/Cy",  out.geometry.Cy);
    out.geometry.NEx = getOrDefault(m, "Geometry/NEx", out.geometry.NEx);
    out.geometry.NEy = getOrDefault(m, "Geometry/NEy", out.geometry.NEy);
    out.geometry.NWx = getOrDefault(m, "Geometry/NWx", out.geometry.NWx);
    out.geometry.NWy = getOrDefault(m, "Geometry/NWy", out.geometry.NWy);
    out.geometry.SEx = getOrDefault(m, "Geometry/SEx", out.geometry.SEx);
    out.geometry.SEy = getOrDefault(m, "Geometry/SEy", out.geometry.SEy);
    out.geometry.SWx = getOrDefault(m, "Geometry/SWx", out.geometry.SWx);
    out.geometry.SWy = getOrDefault(m, "Geometry/SWy", out.geometry.SWy);

    // --- Blending (alpha 0..1) ---
    out.blending.Na  = getOrDefault(m, "Blending/Na",  out.blending.Na);
    out.blending.Sa  = getOrDefault(m, "Blending/Sa",  out.blending.Sa);
    out.blending.Ea  = getOrDefault(m, "Blending/Ea",  out.blending.Ea);
    out.blending.Wa  = getOrDefault(m, "Blending/Wa",  out.blending.Wa);
    out.blending.Ca  = getOrDefault(m, "Blending/Ca",  out.blending.Ca);
    out.blending.NEa = getOrDefault(m, "Blending/NEa", out.blending.NEa);
    out.blending.NWa = getOrDefault(m, "Blending/NWa", out.blending.NWa);
    out.blending.SEa = getOrDefault(m, "Blending/SEa", out.blending.SEa);
    out.blending.SWa = getOrDefault(m, "Blending/SWa", out.blending.SWa);
    clamp(out.blending.Na, 0, 1); clamp(out.blending.Sa, 0, 1);
    clamp(out.blending.Ea, 0, 1); clamp(out.blending.Wa, 0, 1);
    clamp(out.blending.Ca, 0, 1);
    clamp(out.blending.NEa, 0, 1); clamp(out.blending.NWa, 0, 1);
    clamp(out.blending.SEa, 0, 1); clamp(out.blending.SWa, 0, 1);

    // --- Color ---
    out.color.r = getOrDefault(m, "Color/r", out.color.r);
    out.color.g = getOrDefault(m, "Color/g", out.color.g);
    out.color.b = getOrDefault(m, "Color/b", out.color.b);
    clamp(out.color.r, 0, 2); clamp(out.color.g, 0, 2); clamp(out.color.b, 0, 2);

    // --- Cropping ---
    out.cropping.x      = getOrDefault(m, "Cropping/x", out.cropping.x);
    out.cropping.y      = getOrDefault(m, "Cropping/y", out.cropping.y);
    out.cropping.width  = getOrDefault(m, "Cropping/width", out.cropping.width);
    out.cropping.height = getOrDefault(m, "Cropping/height", out.cropping.height);
    if (out.cropping.width < 0) out.cropping.width = 0;
    if (out.cropping.height < 0) out.cropping.height = 0;

    // --- Wayland ---
    out.wayland.useDMABuf = getOrDefault(m, "Wayland/useDMABuf", out.wayland.useDMABuf);
    out.wayland.pipewireNode = getOrDefault(m, "Wayland/pipewireNode", out.wayland.pipewireNode);

    return result;
}

bool saveConfig(const std::string& filename, const ArgusConfig& config) {
    std::map<std::string, std::string> m;

    m["General/Child"] = config.general.child;
    m["General/Prefix"] = config.general.prefix;
    m["General/title"] = config.general.title;
    m["General/method"] = config.general.captureMethod;
    m["General/PillowRec"] = std::to_string(config.general.pillowRec);
    m["General/QuadRec"] = std::to_string(config.general.quadRec);
    m["General/SmoothLen"] = std::to_string(config.general.smoothLen);
    m["General/fps"] = std::to_string(config.general.fps);
    m["General/videoSync"] = config.general.videoSync ? "true" : "false";
    m["General/stats"] = config.general.stats ? "true" : "false";
    m["General/virtualDesktop"] = config.general.virtualDesktop ? "true" : "false";
    m["General/webServerInterface"] = config.general.webServerInterface;
    m["General/webServerPort"] = std::to_string(config.general.webServerPort);

    m["Geometry/Nx"] = std::to_string(config.geometry.Nx);
    m["Geometry/Ny"] = std::to_string(config.geometry.Ny);
    m["Geometry/Sx"] = std::to_string(config.geometry.Sx);
    m["Geometry/Sy"] = std::to_string(config.geometry.Sy);
    m["Geometry/Ex"] = std::to_string(config.geometry.Ex);
    m["Geometry/Ey"] = std::to_string(config.geometry.Ey);
    m["Geometry/Wx"] = std::to_string(config.geometry.Wx);
    m["Geometry/Wy"] = std::to_string(config.geometry.Wy);
    m["Geometry/Cx"] = std::to_string(config.geometry.Cx);
    m["Geometry/Cy"] = std::to_string(config.geometry.Cy);
    m["Geometry/NEx"] = std::to_string(config.geometry.NEx);
    m["Geometry/NEy"] = std::to_string(config.geometry.NEy);
    m["Geometry/NWx"] = std::to_string(config.geometry.NWx);
    m["Geometry/NWy"] = std::to_string(config.geometry.NWy);
    m["Geometry/SEx"] = std::to_string(config.geometry.SEx);
    m["Geometry/SEy"] = std::to_string(config.geometry.SEy);
    m["Geometry/SWx"] = std::to_string(config.geometry.SWx);
    m["Geometry/SWy"] = std::to_string(config.geometry.SWy);

    m["Blending/Na"] = std::to_string(config.blending.Na);
    m["Blending/Sa"] = std::to_string(config.blending.Sa);
    m["Blending/Ea"] = std::to_string(config.blending.Ea);
    m["Blending/Wa"] = std::to_string(config.blending.Wa);
    m["Blending/Ca"] = std::to_string(config.blending.Ca);
    m["Blending/NEa"] = std::to_string(config.blending.NEa);
    m["Blending/NWa"] = std::to_string(config.blending.NWa);
    m["Blending/SEa"] = std::to_string(config.blending.SEa);
    m["Blending/SWa"] = std::to_string(config.blending.SWa);

    m["Color/r"] = std::to_string(config.color.r);
    m["Color/g"] = std::to_string(config.color.g);
    m["Color/b"] = std::to_string(config.color.b);

    m["Cropping/x"] = std::to_string(config.cropping.x);
    m["Cropping/y"] = std::to_string(config.cropping.y);
    m["Cropping/width"] = std::to_string(config.cropping.width);
    m["Cropping/height"] = std::to_string(config.cropping.height);

    m["Wayland/useDMABuf"] = config.wayland.useDMABuf;
    m["Wayland/pipewireNode"] = config.wayland.pipewireNode;

    saveConfiguration(m, filename);
    return true;
}

CaptureMethod parseCaptureMethod(const std::string& s) {
    std::string lower = s;
    std::transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    if (lower == "wayland") return CaptureMethod::Wayland;
    if (lower == "auto") return CaptureMethod::Auto;
    return CaptureMethod::X11;
}

} // namespace argus
