#pragma once
#include <string>
#include <iostream>

#ifdef WIN32
#include <windows.h>
#include <GL/gl.h>
#include <GL/wglext.h>
#include "desktop.h"
#elif __linux__
#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/Xutil.h>
#include <X11/extensions/XShm.h>
#include <X11/Xlib.h>
#include <GL/glx.h>
#include "input.hpp"
#endif

#ifdef WIN32
int main(int argc, char** argv);

extern HINSTANCE   hInstance;
extern HINSTANCE   hPrevInstance;
extern int         nShowCmd;

int __stdcall WinMain(HINSTANCE _hInstance, HINSTANCE _hPrevInstance, char*, int _nShowCmd);
#endif

void usage(int argc, char **argv);
std::string version();
