/**
 *
 * Simple and stupid picture from SHM displayer
 *
 */
#include "shm.h"
#include "desktop.h"
#include "types.h"

#include <iostream>
#include <string>
#include <vector>

#ifdef WIN32

#include <windows.h>

// Déclarations globales
int screenWidth = 0;
int screenHeight = 0;
int bufferSize = 0; // RGBA

// Pointeur vers le buffer RGBA (vous vous en occupez)
unsigned char* rgbaBuffer = nullptr;
bool *inWrite;

// Fonction de traitement des messages
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    // Copie du buffer RGBA vers le contexte de périphérique (DC)
    if (rgbaBuffer != nullptr) {
        BITMAPINFOHEADER bi = {};
        bi.biSize = sizeof(BITMAPINFOHEADER);
        bi.biWidth = screenWidth;
        bi.biHeight = screenHeight; // Négatif pour inverser l'orientation
        bi.biPlanes = 1;
        bi.biBitCount = 32;
        bi.biCompression = BI_RGB;

        while (*inWrite)
            ;
        SetDIBitsToDevice(hdc, 0, 0, screenWidth, screenHeight, 0, 0, 0, screenHeight, rgbaBuffer, (BITMAPINFO*)&bi, DIB_RGB_COLORS); //  - 0x100 ????????????
    }

    EndPaint(hwnd, &ps);
    InvalidateRect(hwnd, NULL, TRUE);
    switch (uMsg) {
    case WM_PAINT: {
        break;
    }
    case WM_KEYDOWN: {
        if (wParam == VK_ESCAPE) {
            PostQuitMessage(0); // Quitter l'application si la touche ESC est pressée
        }
        break;
    }
    case WM_DESTROY: {
        PostQuitMessage(0);
        break;
    }
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Enregistrement de la classe de la fenêtre
    const char className[] = "RGBAWindowClass";
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = className;
    RegisterClass(&wc);

    t_argusExchange* header;
    std::string out0 = "prefix Argus SharedMemory";
    LPVOID shm = getSHM(out0.c_str(), sizeof(*header));
    header = (t_argusExchange*)shm;

    screenWidth = header->width;
    screenHeight = header->height;
    shm = getSHM(out0.c_str(), sizeof(*header) + header->size);
    header = (t_argusExchange*)shm;
    rgbaBuffer = ((unsigned char *) (header) + sizeof(*header));
    inWrite = &header->inWrite;
    // Création de la fenêtre
    HWND hwnd = CreateWindowEx(0, className, "Affichage RGBA", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, screenWidth, screenHeight, nullptr, nullptr, hInstance, nullptr);
    if (hwnd == nullptr) {
        return 0;
    }

    // Affichage de la fenêtre
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Boucle de messages
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}
#elif __linux__
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <sys/shm.h>
#include "input.hpp"
#include "configuration.h"

input* capturer;

void updateImageFromSharedMemory(Display* display, Window window) {
    capturer->shoot();
    XPutImage(display, window, DefaultGC(display, DefaultScreen(display)), capturer->getXimg(), 0, 0, 0, 0, capturer->width, capturer->height);
//    XDestroyImage(capturer->getXimg());
}

int main() {
    std::string out0 = "prefix Argus SharedMemory";

    std::map<std::string, std::string> configuration = readConfiguration("config.ini");

    Display* display = XOpenDisplay(nullptr);
    if (!display) {
        std::cerr << "Failed to open X display." << std::endl;
        return 1;
    }

    capturer = new input(configuration);
    Window window = XCreateSimpleWindow(display, DefaultRootWindow(display), 0, 0, capturer->width, capturer->height, 0, BlackPixel(display, DefaultScreen(display)), WhitePixel(display, DefaultScreen(display)));

    std::string title = configuration["General/title"] + " copy";
    XStoreName(display, window, title.c_str());

    XSelectInput(display, window, ExposureMask);
    XMapWindow(display, window);

    XEvent event;

    while (true)
        updateImageFromSharedMemory(display, window);

    delete capturer;
    XCloseDisplay(display);

    return 0;
}
#endif
