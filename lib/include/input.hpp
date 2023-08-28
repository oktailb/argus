#pragma once

#ifdef WIN32
#include <ddraw.h>
#include <windows.h>
#pragma comment(lib, "d3d9.lib")
#include <d3d9.h>
#pragma comment(lib, "d3dx9.lib")
#include <d3dx9.h>
#include <D3dx9tex.h>
#elif __linux__
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xresource.h>
#include <X11/Xutil.h>
#include <X11/extensions/XTest.h>
#include <X11/keysym.h>
#include <X11/extensions/XShm.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#endif
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include "types.h"

class input
{
public:
    input(std::map<std::string, std::string> &configuration);
    ~input();

    std::map<std::string, std::string>                  &configuration;

    int													width;
	int													height;
    std::string											title;
	bool												full;

    bool                                                captureSingleWindow(char * buffer, int& width, int& height);
    bool                                                captureFullScreen(char * buffer, int& width, int& height);

	void												shoot();
    void                                                configure();

#ifdef WIN32
    bool                                                directX;
    bool                                                gdi;
    HDC													hScrDC;
    HWND												hWindow;
    LPVOID                                              region;
    static LRESULT WINAPI								WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    HWND hWnd;
    BITMAPINFOHEADER bi;
    RECT rect;
    HDC hdcScreen;
    HDC hdcWindow;
    HDC hdcMem;
    HBITMAP hBitmap;
    HBITMAP hBitmapOld;
    bool initGDI(HWND hWndToCapture);
    bool captureGDI(char *buffer);
    void cleanupGDI();

    IDirect3D9*             pD3D;
    IDirect3DDevice9*       pDevice;
    IDirect3DSurface9*      pSurface;
    D3DPRESENT_PARAMETERS   d3dpp;
    D3DLOCKED_RECT          lockedRect;

    int                     nb_adapters;
    int                     nb_screens;

    bool initDirectX(HWND hWndToCapture);
    bool captureDirectX(char *buffer);
    void cleanupDirectX();
    void configureWindow();

#elif __linux__
    void*               region;
    Display *           display;
    Window              root;
    XWindowAttributes   attributes;
    XShmSegmentInfo     shminfo;
    XImage *            ximg;
    bool initXSHM();
    bool captureXSHM();
    void cleanupXSHM();

    void testAlternative();
    char *getWindowName (Display *disp, Window win);
    Window *getWindowList (Display *disp, unsigned long *len);
    void listWindows(Display *disp, std::map<std::string, Window> &listOut);
    void listDisplay(std::map<std::string, Window> &listOut);
    Window windowFromNameSearch(Display *display, Window current, char const *needle);
    Window windowFromPidSearch(Display *display, Window current, unsigned long  _pid);
    XImage *getXimg() const;
#endif
    t_argusExchange     *header;
    int getWidth() const;
    int getHeight() const;
};
