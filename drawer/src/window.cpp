/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "glwidget.h"
#include "window.h"
#include <iostream>
#include <chrono>
#include <map>
#include <string>

#include "shm.h"

#ifdef WIN32
#include <windows.h>
#include <windowsx.h>
#include <winuser.h>
#include <gl/wglext.h>
#include <gl/GL.h>
#endif

// Pointeurs de fonction pour les extensions OpenGL WGL
typedef BOOL(WINAPI* PFNWGLCHOOSEPIXELFORMATARBPROC)(HDC hdc, const int* piAttribIList, const FLOAT* pfAttribFList, UINT nMaxFormats, int* piFormats, UINT* nNumFormats);
typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC hDC, HGLRC hShareContext, const int* attribList);

static bool ready = false;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    ArgusWindow *these = (ArgusWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    switch (message)
    {

    case WM_PAINT:
        these->paintGL();
        break;
    case WM_SIZE:
    {
        int newWidth = LOWORD(lParam);  // Nouvelle largeur de la fenêtre
        int newHeight = HIWORD(lParam); // Nouvelle hauteur de la fenêtre
        if (ready) these->resizeGL(newWidth, newHeight);
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_CHAR:
        these->keyPressEventASCII((int)wParam);
        break;
    case WM_KEYDOWN:
        these->keyPressEvent((int)wParam);
        break;
    case WM_KEYUP:
        these->keyReleaseEvent((int)wParam);
        break;
    case WM_MOUSEMOVE:
    {
        int x = GET_X_LPARAM(lParam); // Obtenir la position X du clic
        int y = GET_Y_LPARAM(lParam); // Obtenir la position Y du clic
        these->mouseMoveEvent(x, y);
        break;
    }
    case WM_LBUTTONDOWN:
    {
        int x = GET_X_LPARAM(lParam); // Obtenir la position X du clic
        int y = GET_Y_LPARAM(lParam); // Obtenir la position Y du clic
        these->mousePressEvent(1, x, y);
        break;
    }
    case WM_LBUTTONUP:
    {
        int x = GET_X_LPARAM(lParam); // Obtenir la position X du clic
        int y = GET_Y_LPARAM(lParam); // Obtenir la position Y du clic
        these->mouseReleaseEvent(1, x, y);
        break;
    }
    default:
        break;
    }
    if (ready) these->paintGL();
    return DefWindowProc(hWnd, message, wParam, lParam);
}
#define GLERR {GLenum error = glGetError(); \
if (error != GL_NO_ERROR) { \
    std::cerr << "OpenGL error: " << error << " on line " << __LINE__ << ": "  << std::endl; \
}\
}\


void ArgusWindow::createGLWindow(const char * title, bool fullscreen)
{
    // 1. Créer la fenêtre
    hInstance = GetModuleHandle(nullptr);
    className = "OpenGLWindow";
    windowTitle = (LPCSTR)(title);
    windowStyle = WS_OVERLAPPEDWINDOW;

    if (fullscreen)
    {
        windowStyle = WS_POPUP | WS_SYSMENU | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
    }

    wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszClassName = "OpenGLWindow";

    RegisterClass(&wc);

    t_argusExchange *header;
    std::string out0 = "prefix Argus SharedMemory";
    LPVOID shm = getSHM(out0.c_str(), sizeof(*header));
    header = (t_argusExchange*) shm;
    width = header->width;
    height = header->height;
    hWnd = CreateWindow(className, title, windowStyle,
                             0, 0, width, height,
                             nullptr, nullptr,
                             hInstance, nullptr);


    // 2. Obtenez le contexte de périphérique (HDC) de la fenêtre
    hDC = GetDC(hWnd);

    // 3. Configurez les attributs du pixel pour le contexte OpenGL
    PIXELFORMATDESCRIPTOR pfd = {};
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;

    int pixelFormat = ChoosePixelFormat(hDC, &pfd);
    SetPixelFormat(hDC, pixelFormat, &pfd);

    // 4. Créez le contexte OpenGL
    hRC = nullptr;

    if (fullscreen)
    {
        hRC = wglCreateContext(hDC);
        wglMakeCurrent(hDC, hRC);
    }
    else
    {
        // Utilisation des extensions WGL pour créer un contexte OpenGL moderne
        PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
        PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

        if (wglChoosePixelFormatARB && wglCreateContextAttribsARB)
        {
            int pixelFormatAttribs[] =
            {
                WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
                WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
                WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
                WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
                WGL_COLOR_BITS_ARB, 32,
                WGL_DEPTH_BITS_ARB, 24,
                WGL_STENCIL_BITS_ARB, 8,
                0
            };

            int pixelFormat;
            UINT numFormats;
            wglChoosePixelFormatARB(hDC, pixelFormatAttribs, nullptr, 1, &pixelFormat, &numFormats);

            const int contextAttribs[] =
            {
                WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
                WGL_CONTEXT_MINOR_VERSION_ARB, 6,
                WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
                0
            };

            hRC = wglCreateContextAttribsARB(hDC, nullptr, contextAttribs);
            wglMakeCurrent(hDC, hRC);
        }
        else
        {
            // Fallback vers le contexte OpenGL traditionnel (non moderne)
            hRC = wglCreateContext(hDC);
            wglMakeCurrent(hDC, hRC);
            GLERR
        }
    }

    // 5. Affichez la fenêtre et entrez dans la boucle de messages
    ShowWindow(hWnd, SW_SHOW);
    SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)this);
    ready = true;
}


ArgusWindow::ArgusWindow(std::map<std::string, std::string> configuration)
{
    shiftPressed = false;
    ctrlPressed = false;
    inMove = false;
    fullscreen = (configuration["General/virtualDesktop"].compare("true") == 0);
    glWidget = new GLWidget(configuration);
}

void ArgusWindow::exec()
{
    createGLWindow("Argus", true);
//    UpdateWindow(hWnd);
    glWidget->initializeGL();
    MSG msg = {};

    auto start = std::chrono::high_resolution_clock::now();
    int counter = 0;
    while (true) {
        auto begin = std::chrono::high_resolution_clock::now();

        if (PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT)
                break;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        paintGL();

        SwapBuffers(hDC);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
        if (duration.count() < 1000/60)
        {
            //Sleep(1000/60 - duration.count());
        }
        counter++;
        if (counter%1000 == 0) {
            auto step = std::chrono::high_resolution_clock::now();
            auto lap = std::chrono::duration_cast<std::chrono::milliseconds>(step - start);
            start = std::chrono::high_resolution_clock::now();
            std::cerr << 1000.0f * counter / lap.count()  << std::endl;
        }
    }
}

ArgusWindow::~ArgusWindow()
{
    // 6. Libérez les ressources à la fin
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(hRC);
    ReleaseDC(hWnd, hDC);
    DestroyWindow(hWnd);
}

HWND ArgusWindow::getGlThread() const
{
    return hWnd;
}

void ArgusWindow::mousePressEvent(int button, int x, int y)
{
    inMove = true;
    glWidget->setLastPos(x, y);
}

void ArgusWindow::mouseMoveEvent(int x, int y)
{
    x = std::min(x, width);
    y = std::min(y, height);
    x = std::max(x, 0);
    y = std::max(y, 0);
    glWidget->setLastPos(x, y);
    if (inMove && glWidget->inEditMode()) glWidget->movePointTo(x, y);
}

void ArgusWindow::mouseReleaseEvent(int button, int x, int y)
{
    inMove = false;
}

void ArgusWindow::keyReleaseEvent(int key)
{
    if (key == VK_SHIFT) {
        shiftPressed = false;
        glWidget->setStep(shiftPressed*ctrlPressed*500 + shiftPressed*100 + ctrlPressed*10 + 1);
    }
    if (key == VK_CONTROL) {
        ctrlPressed = false;
        glWidget->setStep(shiftPressed*ctrlPressed*500 + shiftPressed*100 + ctrlPressed*10 + 1);
    }
}

void ArgusWindow::keyPressEventASCII(char key)
{
    if (key == 'e') {
        glWidget->toggleEditMode();
    }
    else if (key == 'r') {
        glWidget->adjustR(ctrlPressed);
    }
    else if (key == 'g') {
        glWidget->adjustG(ctrlPressed);
    }
    else if (key == 'b') {
        glWidget->adjustB(ctrlPressed);
    }
    else if (key == 's') {
        glWidget->save("config.ini");
    }
    else if ((key >= '1') && (key <= '9')) {
        glWidget->selectPoint(key - '0');
    }
    else if ((key == '+') || (key == '=')) {
        glWidget->increasePillowRecursion();
    }
    else if (key == '-') {
        glWidget->decreasePillowRecursion();
    }
    else {
        std::cerr << "KEY CHAR == " << key << std::endl;
    }

}
void ArgusWindow::keyPressEvent(int key)
{
    if (key == VK_ESCAPE) {
        exit(0);
    }
    else if (key == VK_HOME) {
        glWidget->increaseSmoothLen();
    }
    else if (key == VK_END) {
        glWidget->decreaseSmoothLen();
    }
    else if (key == VK_PRIOR) {
        glWidget->increaseAlpha();
    }
    else if (key == VK_NEXT) {
        glWidget->decreaseAlpha();
    }
    else if (key == VK_UP) {
        glWidget->movePointUp();
    }
    else if (key == VK_DOWN) {
        glWidget->movePointDown();
    }
    else if (key == VK_LEFT) {
        glWidget->movePointLeft();
    }
    else if (key ==VK_RIGHT) {
        glWidget->movePointRight();
    }
    else if (key == VK_SHIFT) {
        shiftPressed = true;
        glWidget->setStep(shiftPressed*ctrlPressed*500 + shiftPressed*100 + ctrlPressed*10 + 1);
    }
    else if (key == VK_CONTROL) {
        ctrlPressed = true;
        glWidget->setStep(shiftPressed*ctrlPressed*500 + shiftPressed*100 + ctrlPressed*10 + 1);
    }
    else {
        std::cerr << "KEY == " << key << std::endl;
    }
}

