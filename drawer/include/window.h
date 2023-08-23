#pragma once

#include <map>
#include <string>
#include "glwidget.h"

class ArgusWindow
{

public:
    ArgusWindow(std::map<std::string, std::string> configuration);
    ~ArgusWindow();

    HWND getGlThread() const;

    void exec();
    void mousePressEvent(int button, int x, int y);
    void mouseMoveEvent(int x, int y);
    void mouseReleaseEvent(int button, int x, int y);
    void keyPressEventASCII(char key);
    void keyPressEvent(int key);
    void keyReleaseEvent(int key);
    void resizeGL(int width, int height) {glWidget->resizeGL(width, height);}
    void paintGL() {glWidget->paintGL();}
private:
    GLWidget *  glWidget;
    bool        shiftPressed;
    bool        ctrlPressed;
    bool        inMove;
    bool        fullscreen;
    int         width;
    int         height;
#ifdef WIN32
    HINSTANCE hInstance;
    LPCSTR className;
    LPCSTR windowTitle;
    DWORD windowStyle;
    WNDCLASS wc;
    HWND hWnd;
    HDC hDC;
    HGLRC hRC;
    PIXELFORMATDESCRIPTOR pfd;
#elif __linux__
#endif
    void createGLWindow(const char * title, bool fullscreen);

};
