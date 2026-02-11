#pragma once

/**
 * @file argusConfig.h
 * @brief Configuration typée, validée et maintenable pour Argus (edge blending).
 *
 * Remplace l'accès direct au map INI par des structures typées avec valeurs par défaut
 * et validation. Lecture/écriture INI conservée pour compatibilité.
 */

#include <string>
#include <cstdint>

namespace argus {

/** Méthode de capture (Linux). */
enum class CaptureMethod {
    X11,
    Wayland,
    Auto  // X11 si DISPLAY, sinon Wayland si WAYLAND_DISPLAY
};

/** Résultat de chargement avec message d'erreur éventuel. */
struct ConfigLoadResult {
    bool ok = false;
    std::string error;
};

// --- Général ---
struct GeneralConfig {
    std::string child;
    std::string prefix{"prefix"};
    std::string title;
    std::string captureMethod{"x11"};  // "x11" | "wayland" | "auto"
    int pillowRec{4};
    int quadRec{1};
    float smoothLen{0.01f};
    int fps{60};
    bool videoSync{false};
    bool stats{false};
    bool virtualDesktop{false};
    // Web UI
    std::string webServerInterface{"127.0.0.1"};
    int webServerPort{8080};
};

// --- Géométrie (grille 3×3 : N, S, E, W, C, NE, NW, SE, SW) ---
struct GeometryConfig {
    double Nx{0}, Ny{0}, Sx{0}, Sy{0}, Ex{0}, Ey{0}, Wx{0}, Wy{0};
    double Cx{0}, Cy{0};
    double NEx{0}, NEy{0}, NWx{0}, NWy{0}, SEx{0}, SEy{0}, SWx{0}, SWy{0};
};

// --- Blending (alpha par sommet) ---
struct BlendingConfig {
    double Na{1}, Sa{1}, Ea{1}, Wa{1}, Ca{1};
    double NEa{1}, NWa{1}, SEa{1}, SWa{1};
};

// --- Couleur globale (gain RGB) ---
struct ColorConfig {
    double r{1}, g{1}, b{1};
};

// --- Recadrage source ---
struct CroppingConfig {
    int x{0}, y{0}, width{0}, height{0};
};

// --- Wayland ---
struct WaylandConfig {
    std::string useDMABuf{"auto"}; // "auto", "true" (force), "false" (disable)
    std::string pipewireNode;
};

/** Configuration complète Argus. */
struct ArgusConfig {
    GeneralConfig general;
    GeometryConfig geometry;
    BlendingConfig blending;
    ColorConfig color;
    CroppingConfig cropping;
    WaylandConfig wayland;
};

/**
 * Charge et valide la configuration depuis un fichier INI.
 * Valeurs manquantes remplacées par défauts; clés invalides ignorées avec avertissement.
 */
ConfigLoadResult loadConfig(const std::string& filename, ArgusConfig& out);

/**
 * Sauvegarde la configuration dans un fichier INI.
 * Retourne false en cas d'erreur d'écriture.
 */
bool saveConfig(const std::string& filename, const ArgusConfig& config);

/**
 * Parse une chaîne en CaptureMethod.
 */
CaptureMethod parseCaptureMethod(const std::string& s);

} // namespace argus
