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
#include <strsafe.h>

HANDLE hMapFile = nullptr;
t_argusExchange* header = nullptr;

/**
 * @brief updateImageFromSharedMemory
 * @param hdc
 * @param hBitmap
 * @brief Collect the shm picture data and produce am Operating syste display server compatible bitmap
 */
void updateImageFromSharedMemory(HDC hdc, HBITMAP hBitmap) {
    BITMAPINFO bmpInfo = {};
    bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfo.bmiHeader.biWidth = header->width;
    bmpInfo.bmiHeader.biHeight = -header->height;
    bmpInfo.bmiHeader.biPlanes = 1;
    bmpInfo.bmiHeader.biBitCount = 32;
    bmpInfo.bmiHeader.biCompression = BI_RGB;

    SetDIBitsToDevice(hdc, 0, 0, header->width, header->height, 0, 0, 0, header->height, (BYTE*)header + sizeof(t_argusExchange), &bmpInfo, DIB_RGB_COLORS);
}

void ErrorExit(LPTSTR lpszFunction)
{
    // Retrieve the system error message for the last-error code

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError();

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message and exit the process

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
    StringCchPrintf((LPTSTR)lpDisplayBuf,
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"),
        lpszFunction, dw, lpMsgBuf);
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
    ExitProcess(dw);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    switch(Msg)
    {
    case WM_DESTROY:
        PostQuitMessage(WM_QUIT);
        break;
        break;
    default:
        return DefWindowProc(hwnd, Msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
//int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, wchar_t * lpCmdLine, int nCmdShow)
{
    std::string out0 = "prefix Argus SharedMemory";

    LPVOID shm = getSHM(out0.c_str(), sizeof(*header));
    header = (t_argusExchange*)shm;
    shm = getSHM(out0.c_str(), header->size + sizeof(*header));

    // Register the window class.
    const char CLASS_NAME[]  = "OpenGLWindow";

    WNDCLASS wc = { };

    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, CLASS_NAME, "Image Viewer", WS_OVERLAPPEDWINDOW,
                               CW_USEDEFAULT, CW_USEDEFAULT, header->width, header->height, NULL, NULL, GetModuleHandle(NULL), NULL);
    if (hwnd == NULL) {
        ErrorExit(TEXT("CreateWindowEx"));
        std::cerr << "Failed to create window." << std::endl;
        return 1;
    }
    ShowWindow(hwnd, nShowCmd);
    HDC hdc = GetDC(hwnd);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, header->width, header->height);

    while (true) {
        updateImageFromSharedMemory(hdc, hBitmap);
        Sleep(10);
    }

    DeleteObject(hBitmap);
    ReleaseDC(hwnd, hdc);
    UnmapViewOfFile(header);
    CloseHandle(hMapFile);

    return 0;
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
