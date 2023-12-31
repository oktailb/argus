#pragma once

#include <map>
#include <string>
#include "glwidget.h"

class ArgusWindow
{

public:
    ArgusWindow(std::string filename);
    ~ArgusWindow();

    void exec();
    void paintGL() {glWidget->paintGL();}
    void resizeGL(int width, int height) {glWidget->resizeGL(width, height);}
    void mousePressEvent(int button, int x, int y);
    void mouseMoveEvent(int x, int y);
    void mouseReleaseEvent(int button, int x, int y);
#ifdef WIN32
    void keyPressEventASCII(char key);
#endif
    void keyPressEvent(int key);
    void keyReleaseEvent(int key);


private:
    std::map<std::string, std::string> configuration;
    int         fps;
    double      delayMs;
    std::string filename;
    GLWidget *  glWidget;
    bool        shiftPressed;
    bool        ctrlPressed;
    bool        inMove;
    bool        fullscreen;
    bool        videoSync;
    bool        stats;
    int         width;
    int         height;
    bool        ready;
#ifdef WIN32
    HWND        getGlThread() const;

    HINSTANCE   hInstance;
    LPCSTR      className;
    LPCSTR      windowTitle;
    DWORD       windowStyle;
    WNDCLASS    wc;
    HWND        hWnd;
    HDC         hDC;
    HGLRC       hRC;
    PIXELFORMATDESCRIPTOR pfd;
#elif __linux__
    Display* display = nullptr;
    Window window = 0;
    XEvent event = {};
    GLXContext context = nullptr;
#endif
    void eventLoop();
    void createGLWindow(const char * title, bool fullscreen);
};
