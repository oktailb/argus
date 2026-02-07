#include <input.hpp>
#include "shm.h"
#include "configuration.h"
#include "argusConfig.h"
#include "types.h"
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <algorithm>
#include <chrono>

#ifdef WIN32
#include <windows.h>
#include <DXGI.h>
#include <d3dx9tex.h>
#include <windows.h>
#pragma comment(lib, "d3d9.lib")
#include <d3d9.h>
#include <dwmapi.h>
#define STRICT  1
#include <windows.h>
#include <d3d11.h>
#include <d3d11sdklayers.h>
#include <dxgi.h>
#include <d3dcommon.h>
#include <D3Dcompiler.h>
#include <Windows.h>
#include <psapi.h>		// NT only!
#pragma comment(lib, "psapi")	// NT only!
#elif __linux__
#include <X11/X.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/Xrandr.h>
#include <dirent.h>
#include <string.h>
#endif

using namespace std;

std::vector<std::string> removeDuplicates(const std::vector<std::string>& inputVector)
{
    std::vector<std::string> resultVector;

    for (const std::string& str : inputVector) {
        if (std::find(resultVector.begin(), resultVector.end(), str) == resultVector.end()) {
            resultVector.push_back(str);
        }
    }
    std::sort(resultVector.begin(), resultVector.end());
    return resultVector;
}

#ifdef WIN32
void GetOpenWindows(std::map<std::string, HWND> & windowList)
{
    // Enumerate through all top-level windows
    EnumWindows([](HWND hwnd, LPARAM lParam)
    {
        std::map<std::string, HWND>& windowList = *reinterpret_cast<std::map<std::string, HWND>*>(lParam);

        const int titleLength = GetWindowTextLength(hwnd);
        if (titleLength > 0)
        {
            std::vector<char> titleBuffer(titleLength + 1);
            GetWindowTextA(hwnd, titleBuffer.data(), titleLength + 1);
            HWND CheckhWnd = FindWindowA(nullptr, (LPSTR)titleBuffer.data());
            if (CheckhWnd != nullptr)
                windowList[titleBuffer.data()] = hwnd;
        }

        return TRUE;
    }, reinterpret_cast<LPARAM>(&windowList));
    return;
}

LRESULT WINAPI input::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        exit(0);
        return(0);
    }
    return(DefWindowProc(hwnd, msg, wParam, lParam));
}

void input::configureWindow()
{
    hWnd = FindWindowA(nullptr, (LPCSTR)title.c_str());
    if (hWnd == nullptr) {
        std::map<std::string, HWND> list;
        GetOpenWindows(list);
        int i = 1;
        for (auto it = list.begin(); it != list.end() ; it++)
            cerr << i++ << ") " << it->first << endl;
        std::cerr << "wich one ? ";
        int answer = 0;
        cin >> answer;
        auto it = list.begin();
        for (i = 1; i < list.size() ; i++)
        {
            if (i == answer)
            {
                hWnd = it->second;
                title = it->first;
                argus::ArgusConfig cfg;
                if (argus::loadConfig(filename, cfg).ok) {
                    cfg.general.title = title;
                    argus::saveConfig(filename, cfg);
                }
                break;
            }
            it++;
        }
        FindWindowW(nullptr, (LPCWSTR)title.c_str());
    }
    GetClientRect(hWnd, &rect);
    width = rect.right - rect.left;
    height = rect.bottom - rect.top;
}

input::input(std::string filename)
    :
      filename(filename)
{
    argus::ArgusConfig config;
    argus::loadConfig(filename, config);

    title = config.general.title;
    std::string method = config.general.captureMethod;
    full = (title.compare("full") == 0);
    directX = (method.compare("directx") == 0);
    gdi = (method.compare("gdi") == 0);
    fps = config.general.fps;
    delayMs = 1000.0f / fps;
    videoSync = config.general.videoSync;
    stats = config.general.stats;

    cropX       = config.cropping.x;
    cropY       = config.cropping.y;
    cropWidth   = config.cropping.width;
    cropHeight  = config.cropping.height;

    configureWindow();

    if (full)
    {
        width = GetSystemMetrics(SM_CXSCREEN);
        height = GetSystemMetrics(SM_CYSCREEN);

        hdcScreen = GetDC(nullptr);

        hdcMem = CreateCompatibleDC(hdcScreen);
        hBitmap = CreateCompatibleBitmap(hdcScreen, width, height);
        hBitmapOld = (HBITMAP)SelectObject(hdcMem, hBitmap);

        bi.biSize = sizeof(BITMAPINFOHEADER);
        bi.biWidth = width;
        bi.biHeight = -height;
        bi.biPlanes = 1;
        bi.biBitCount = 32;
        bi.biCompression = BI_RGB;
        bi.biSizeImage = 0;
        bi.biXPelsPerMeter = 0;
        bi.biYPelsPerMeter = 0;
        bi.biClrUsed = 0;
        bi.biClrImportant = 0;
    }
    else
    {
        if (gdi) initGDI(hWnd);
    }
    if (directX) initDirectX(hWnd);

    std::string out0 = config.general.prefix + " Argus SharedMemory";

    const int SHM_SIZE = 2 * (width*height*4) + sizeof(*header);

    region = createSHM(out0.c_str(), SHM_SIZE);
    header = (t_argusExchange*)region;
    header->width = width;
    header->height = height;
    header->size = width*height*4;
    header->firstBufferInWrite = false;
}

input::~input()
{
    cleanupGDI();
    if (directX) cleanupDirectX();
}

void input::shoot()
{
    static auto start = std::chrono::high_resolution_clock::now();
    static int counter = 0;
    auto begin = std::chrono::high_resolution_clock::now();

#ifdef WIN32
    header->firstBufferInWrite = !header->firstBufferInWrite;

    char * buffer;
    if (header->firstBufferInWrite)
        buffer = (char*)region + sizeof(*header);
    else
        buffer = (char*)region + sizeof(*header) + header->size;

    if (full)
        captureFullScreen(buffer, width, height);
    else
        captureSingleWindow(buffer, width, height);
#elif __linux__
    captureSingleWindow((char*)region + sizeof(*header), width, height);
#endif

    if (videoSync)
    {
        auto end = std::chrono::high_resolution_clock::now();
        auto unusedMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() - delayMs;
        if (unusedMs > 0)
        {
#ifdef WIN32
            Sleep(unusedMs);
#elif __linux__
            usleep(1000 * unusedMs);
#endif

        }
    }
    counter++;
    if (stats && counter%1000 == 0) {
        auto step = std::chrono::high_resolution_clock::now();
        auto lap = std::chrono::duration_cast<std::chrono::milliseconds>(step - start);
        std::cerr << "Capture FPS: " << 1000.0f * counter / lap.count()  << std::endl;
    }

}

bool input::captureFullScreen(char * buffer, int& width, int& height)
{
    if (!BitBlt(hdcMem, 0, 0, width, height, hdcScreen, 0, 0, SRCCOPY)) {
        DeleteDC(hdcMem);
        ReleaseDC(nullptr, hdcScreen);

        std::cerr << "BitBlt failed" << std::endl;
        return false;
    }

    GetDIBits(hdcMem, hBitmap, 0, height, buffer, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

    return true;
}

bool input::captureSingleWindow(char * buffer, int& width, int& height) {
    if (directX) captureDirectX(buffer);
    else if(gdi) captureGDI(buffer);

    return true;
}
#elif __linux__

void GetOpenWindows(std::map<std::string, Window*> & windowList)
{
    return;
}

void input::testAlternative()
{
    Display *disp;
    XRRScreenResources *context;
    XRROutputInfo *info;
    XRRCrtcInfo *crtc_info;
    int iscres;
    int icrtc;

    disp = XOpenDisplay(0);
    context = XRRGetScreenResources (disp, DefaultRootWindow(disp));
    for (iscres = context->noutput; iscres > 0; )
    {
        --iscres;

        info = XRRGetOutputInfo (disp, context, context->outputs[iscres]);
        if (info->connection == RR_Connected)
        {
            for (icrtc = info->ncrtc; icrtc > 0;)
            {
                --icrtc;

                crtc_info = XRRGetCrtcInfo (disp, context, context->crtcs[icrtc]);
                XRRFreeCrtcInfo(crtc_info);
            }
        }
        XRRFreeOutputInfo (info);
    }
    XRRFreeScreenResources(context);
}


char *input::getWindowName (Display *disp, Window win)
{
    Atom prop = XInternAtom(disp,"WM_NAME",False), type;
    int form;
    unsigned long remain, len;
    unsigned char *list;


    if (XGetWindowProperty(disp, win, prop, 0, 1024, False, AnyPropertyType,
                           &type, &form, &len, &remain, &list) != Success)
    {

        return NULL;
    }

    return (char*)list;
}

Window *input::getWindowList (Display *disp, unsigned long *len)
{
    Atom prop = XInternAtom(disp,"_NET_CLIENT_LIST",False), type;
    int form;
    unsigned long remain;
    unsigned char *list;

    if (XGetWindowProperty(disp,XDefaultRootWindow(disp),prop,0,1024,False,XA_WINDOW,
                           &type,&form,len,&remain,&list) != Success)
    {
        return 0;
    }

    return (Window*)list;
}

void input::listWindows(Display *disp, std::map<std::string, Window> &listOut)
{
    int i;
    unsigned long len;
    Window *list;
    char *name;

    list = (Window*)getWindowList(disp,&len);
    for (i=0;i<(int)len;i++)
    {
        name = getWindowName(disp,list[i]);
        XWindowAttributes attributes = {0};
        std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
        XGetWindowAttributes(disp, list[i], &attributes);
        std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
        listOut[name] = windowFromNameSearch(disp, XDefaultRootWindow(disp), name);
        free(name);
    }
}

void input::listDisplay(std::map<std::string, Window> &listOut)
{
    DIR* d = opendir("/tmp/.X11-unix");

    if (d != NULL)
    {
        struct dirent *dr;
        while ((dr = readdir(d)) != NULL)
        {
            if (dr->d_name[0] != 'X')
                continue;
            char display_name[64] = ":";
            strcat(display_name, dr->d_name + 1);

            Display *disp = XOpenDisplay(display_name);
            if (disp != NULL)
            {
                listWindows(disp, listOut);
                XCloseDisplay(disp);
            }
        }
        closedir(d);
    }
    testAlternative();
}


Window input::windowFromNameSearch(Display *display, Window current, char const *needle)
{
    Window retval, root, parent, *children;
    unsigned children_count;
    char *name = NULL;

    /* Check if this window has the name we seek */
    if(XFetchName(display, current, &name) > 0)
    {
        int r = strcmp(needle, name);
        XFree(name);
        if(r == 0)
        {
            return current;
        }
    }

    retval = 0;

    /* If it does not: check all subwindows recursively. */
    if(0 != XQueryTree(display, current, &root, &parent, &children, &children_count))
    {
        unsigned i;
        for(i = 0; i < children_count; ++i)
        {
            Window win = windowFromNameSearch(display, children[i], needle);

            if(win != 0)
            {
                retval = win;
                break;
            }
        }

        XFree(children);
    }

    return retval;
}




Window input::windowFromPidSearch(Display *display, Window current, unsigned long  _pid)
{
    // Get the PID for the current Window.
    Atom            type;
    int             format;
    unsigned long   nItems;
    unsigned long   bytesAfter;
    unsigned char * propPID = 0;
    Atom            _atomPID;
    Window          retval, root, parent, *children;
    unsigned        children_count;

    retval = 0;
    _atomPID = XInternAtom(display, "_NET_WM_PID", True);
    if(_atomPID == None)
    {
        printf("No such atom\n");
        return current;
    }

    if(Success == XGetWindowProperty(display, current, _atomPID, 0, 1, False, XA_CARDINAL,
                                      &type, &format, &nItems, &bytesAfter, &propPID))
    {
        if(propPID != 0)
        {
            // If the PID matches, add this window to the result set.
            if(_pid == *((unsigned long *)propPID))
            {
                XFree(propPID);
                return(current);
            }
            XFree(propPID);
        }
    }

    /* If it does not: check all subwindows recursively. */
    if(0 != XQueryTree(display, current, &root, &parent, &children, &children_count))
    {
        unsigned i;
        for(i = 0; i < children_count; ++i)
        {
            Window win = windowFromPidSearch(display, children[i], _pid);

            if(win != 0)
            {
                retval = win;
                break;
            }
        }

        XFree(children);
    }

    return retval;
}

void input::configure()
{
    std::map<std::string, Window> list;
    display = XOpenDisplay(":0");
    if (display == NULL)
    {
        // listDisplay(list); // Avoid listing if we aren't going to ask the user
        std::cerr << "Failed to open X display ':0'" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    // Attempt to find the window by title
    root = windowFromNameSearch(display, XDefaultRootWindow(display), title.c_str());
    
    // If not found, try to find a window that *contains* the title substring (optional improvement, or just fail)
    // For now, adhering to strict plan: failing if not found instead of blocking.
    
    if (root == 0)
    {
        std::cerr << "Window '" << title << "' not found." << std::endl;
        
        // Optional: List available windows to stderr for debugging purposes, but do NOT block.
        listDisplay(list);
        std::cerr << "Available windows:" << std::endl;
        int i = 0;
        for (auto it = list.begin() ; it != list.end() ; it++) {
             std::cerr << i++ << ") " << it->first << std::endl;
        }

        std::cerr << "Please update config.ini with a valid window title." << std::endl;
        exit(EXIT_FAILURE); 
    }
    
    //attributes = {0};
    XGetWindowAttributes(display, root, &attributes);
    width = attributes.width;
    height = attributes.height;
}

XImage *input::getXimg() const
{
    return ximg;
}

input::CaptureBuffer input::getCaptureBuffer() const
{
#ifdef ENABLE_WAYLAND
    if (useWayland && waylandCapturer)
        return { waylandCapturer->getData(), waylandCapturer->getWidth(), waylandCapturer->getHeight() };
#endif
    if (ximg)
        return { reinterpret_cast<const unsigned char*>(ximg->data), width, height };
    return { nullptr, 0, 0 };
}

input::input(std::string filename)
    :
    filename(filename)
{
#ifdef __linux__
    waylandCapturer = nullptr;
    useWayland = false;
#endif
#ifdef WIN32
    (void)filename;
    /* Windows uses the first input::input() constructor above. */
#else
    argus::ArgusConfig config;
    argus::ConfigLoadResult loadResult = argus::loadConfig(filename, config);
    if (!loadResult.ok) {
        std::cerr << "Argus config: " << loadResult.error << std::endl;
        exit(EXIT_FAILURE);
    }
    title = config.general.title;
    full = (title.compare("full") == 0);
    fps = config.general.fps;
    delayMs = 1000.0f / fps;
    videoSync = config.general.videoSync;
    stats = config.general.stats;
    cropX = config.cropping.x;
    cropY = config.cropping.y;
    cropWidth = config.cropping.width;
    cropHeight = config.cropping.height;

#ifdef ENABLE_WAYLAND
    argus::CaptureMethod method = argus::parseCaptureMethod(config.general.captureMethod);
    if (method == argus::CaptureMethod::Wayland ||
        (method == argus::CaptureMethod::Auto && getenv("WAYLAND_DISPLAY") != nullptr)) {
        waylandCapturer = new InputWayland();
        if (waylandCapturer->initialize()) {
            useWayland = true;
            width = waylandCapturer->getWidth();
            height = waylandCapturer->getHeight();
        } else {
            delete waylandCapturer;
            waylandCapturer = nullptr;
            if (method == argus::CaptureMethod::Wayland)
                std::cerr << "Argus: method=wayland but Wayland capture not available; use method=x11 to capture an X11/XWayland window." << std::endl;
        }
    }
#endif
    if (!useWayland) {
        configure();
        initXSHM();
    }
#endif
}

input::~input()
{
#ifdef ENABLE_WAYLAND
    if (waylandCapturer) {
        delete waylandCapturer;
        waylandCapturer = nullptr;
    }
#endif
#ifdef __linux__
    if (!useWayland)
        cleanupXSHM();
#endif
}


void input::shoot()
{
    auto begin = std::chrono::high_resolution_clock::now();
#ifdef ENABLE_WAYLAND
    if (waylandCapturer) {
        waylandCapturer->captureCheck();
        // Copy data to SHM or texture
    } else 
#endif
    captureXSHM();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    if (videoSync && duration.count() < 1000.0/fps)
    {
        usleep(1000000.0/fps - duration.count());
    }
}

bool input::captureFullScreen(char * buffer, int& width, int& height)
{
    captureXSHM();
    return true;
}

bool input::captureSingleWindow(char * buffer, int& width, int& height) {
    captureXSHM();
    return true;
}
#endif

int input::getWidth() const
{
    return width;
}

int input::getHeight() const
{
    return height;
}
