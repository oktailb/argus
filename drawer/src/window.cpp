#define XK_MISCELLANY

#include "glwidget.h"
#include "window.h"
#include <iostream>
#include <chrono>
#include <map>
#include <string>
#include <GL/gl.h>
#include <unistd.h>

#include "shm.h"
#include "configuration.h"

#ifdef __linux__
#include "input.hpp"
#endif

#ifdef WIN32
#include <windows.h>
#include <windowsx.h>
#include <winuser.h>
#include <gl/wglext.h>

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

#elif __linux__
#include <X11/Xlib.h>
#include <GL/glx.h>

int WaitForNotify(Display* display, XEvent* event, XPointer arg)
{
    return (event->type == MapNotify) && (event->xmap.window == *((Window*)arg));
}
#endif

void ArgusWindow::createGLWindow(const char * title, bool fullscreen)
{
#ifdef WIN32
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

#elif __linux__
    display = XOpenDisplay(nullptr);

    if (!display)
    {
        std::cerr << "Failed to open X display." << std::endl;
        return;
    }

    Window root = DefaultRootWindow(display);
    XEvent event;

    GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
    XVisualInfo* vi = glXChooseVisual(display, DefaultScreen(display), att);

    if (!vi)
    {
        std::cerr << "No appropriate visual found." << std::endl;
        return;
    }

    Colormap cmap = XCreateColormap(display, RootWindow(display, vi->screen), vi->visual, AllocNone);

    XSetWindowAttributes swa;
    swa.colormap = cmap;
    swa.event_mask = ExposureMask | KeyPressMask;

    window = XCreateWindow(display, RootWindow(display, vi->screen), 0, 0, width, height, 0, vi->depth, InputOutput, vi->visual, CWColormap | CWEventMask, &swa);

    XStoreName(display, window, title);
    XMapWindow(display, window);
    //XIfEvent(display, &event, WaitForNotify, reinterpret_cast<XPointer>(&window));

    context = glXCreateContext(display, vi, nullptr, GL_TRUE);
    glXMakeCurrent(display, window, context);

    XSelectInput(display, window,     ButtonPressMask
                                    | ButtonReleaseMask
                                    | PointerMotionMask
                                    | PointerMotionMask
                                    | KeyPressMask
                                    | KeyReleaseMask);
#endif
    ready = true;
}

ArgusWindow::ArgusWindow(std::string filename)
    :
    filename(filename)
{
    configuration = readConfiguration(filename);
    shiftPressed = false;
    ctrlPressed = false;
    inMove = false;
    fullscreen = (configuration["General/virtualDesktop"].compare("true") == 0);
    glWidget = new GLWidget(filename);
#ifdef __linux__
    width = glWidget->getCapturer()->getWidth();
    height = glWidget->getCapturer()->getHeight();
#endif
    fps = std::stoi(configuration["General/fps"]);
    delayMs = 1000.0f / fps;
    videoSync = (configuration["General/videoSync"].compare("true") == 0);
    stats = (configuration["General/stats"].compare("true") == 0);
}

void ArgusWindow::eventLoop()
{
#ifdef WIN32
    MSG msg = {};
    if (PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT)
            exit(EXIT_SUCCESS);

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
#elif __linux__
    if (XPending(display))
    {
        XNextEvent(display, &event);
        switch (event.type)
        {

        case KeyPress:
        {
            KeySym key = XLookupKeysym(&event.xkey, 0);
            keyPressEvent(key);
            break;
        }

        case KeyRelease:
        {
            KeySym key = XLookupKeysym(&event.xkey, 0);
            keyReleaseEvent(key);
            break;
        }

        case ButtonPress:
        {
            int x = event.xbutton.x;
            int y = event.xbutton.y;
            if (event.xbutton.button == Button1)
            {
                mousePressEvent(1, x, y);
            }
            break;
        }

        case ButtonRelease:
        {
            int x = event.xbutton.x;
            int y = event.xbutton.y;
            if (event.xbutton.button == Button1)
            {
                mouseReleaseEvent(1, x, y);
            }
            break;
        }

        case MotionNotify:
        {
            int x = event.xmotion.x;
            int y = event.xmotion.y;
            mouseMoveEvent(x, y);
            break;
        }

        default:
            break;
        }
    }

#endif
}

void ArgusWindow::exec()
{
    createGLWindow("Argus", true);
    glWidget->initializeGL();

    auto start = std::chrono::high_resolution_clock::now();
    int counter = 0;

    while (true) {
        auto begin = std::chrono::high_resolution_clock::now();

        eventLoop();
        paintGL();

#ifdef WIN32
        SwapBuffers(hDC);
#elif __linux__
        glXSwapBuffers(display, window);
#endif

        if (videoSync)
        {
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
            if (duration.count() < delayMs)
            {
#ifdef WIN32
                Sleep(delayMs - duration.count());
#elif __linux__
                usleep(1000*(delayMs - duration.count()));
#endif
            }
        }
        counter++;
        if (stats && counter%1000 == 0) {
            auto step = std::chrono::high_resolution_clock::now();
            auto lap = std::chrono::duration_cast<std::chrono::milliseconds>(step - start);
            std::cerr << "Draw FPS: " << 1000.0f * counter / lap.count()  << std::endl;
        }
    }
}

ArgusWindow::~ArgusWindow()
{
    if (ready)
    {
#ifdef WIN32
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(hRC);
        ReleaseDC(hWnd, hDC);
        DestroyWindow(hWnd);
#elif __linux__
        glXMakeCurrent(display, None, nullptr);
        glXDestroyContext(display, context);
        XDestroyWindow(display, window);
        XCloseDisplay(display);
#endif
    }
}

#ifdef WIN32
HWND ArgusWindow::getGlThread() const
{
    return hWnd;
}
#endif

void ArgusWindow::mousePressEvent(int button, int x, int y)
{
    inMove = true;
    if (glWidget->inEditMode())
        glWidget->setLastPos(x, y);
}

void ArgusWindow::mouseMoveEvent(int x, int y)
{
    x = std::min(x, width);
    y = std::min(y, height);
    x = std::max(x, 0);
    y = std::max(y, 0);

    if (inMove && glWidget->inEditMode())
    {
        glWidget->setLastPos(x, y);
        glWidget->movePointTo(x, y);
    }
}

void ArgusWindow::mouseReleaseEvent(int button, int x, int y)
{
    inMove = false;
}

#ifdef WIN32

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
    if (key == 'c') {
        glWidget->enableCaptureMode();
    }
    else if (key == 'd') {
        glWidget->enableDebugMode();
    }
    else if (key == 'e') {
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
        glWidget->save(filename);
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
#elif __linux__


void ArgusWindow::keyReleaseEvent(int key)
{
    if (key == XK_Shift_L) {
        shiftPressed = false;
        glWidget->setStep(shiftPressed*ctrlPressed*500 + shiftPressed*100 + ctrlPressed*10 + 1);
    }
    if (key == XK_Control_L) {
        ctrlPressed = false;
        glWidget->setStep(shiftPressed*ctrlPressed*500 + shiftPressed*100 + ctrlPressed*10 + 1);
    }
}

void ArgusWindow::keyPressEvent(int key)
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
        glWidget->save(filename);
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
    else if (key == XK_Escape) {
        exit(0);
    }
    else if (key == XK_Home) {
        glWidget->increaseSmoothLen();
    }
    else if (key == XK_End) {
        glWidget->decreaseSmoothLen();
    }
    else if (key == XK_Page_Up) {
        glWidget->increaseAlpha();
    }
    else if (key == XK_Page_Down) {
        glWidget->decreaseAlpha();
    }
    else if (key == XK_Up) {
        glWidget->movePointUp();
    }
    else if (key == XK_Down) {
        glWidget->movePointDown();
    }
    else if (key == XK_Left) {
        glWidget->movePointLeft();
    }
    else if (key ==XK_Right) {
        glWidget->movePointRight();
    }
    else if (key == XK_Shift_L) {
        shiftPressed = true;
        glWidget->setStep(shiftPressed*ctrlPressed*500 + shiftPressed*100 + ctrlPressed*10 + 1);
    }
    else if (key == XK_Control_L) {
        ctrlPressed = true;
        glWidget->setStep(shiftPressed*ctrlPressed*500 + shiftPressed*100 + ctrlPressed*10 + 1);
    }
    else {
        std::cerr << "KEY == " << key << std::endl;
    }
}
#endif
