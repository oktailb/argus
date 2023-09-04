#pragma once
#include <string>
#include <iostream>

#ifdef WIN32
#include <windows.h>
#include "desktop.h"
#elif __linux__
#endif

#ifdef WIN32
int main(int argc, char** argv);

HINSTANCE   hInstance;
HINSTANCE   hPrevInstance;
int         nShowCmd;

int __stdcall WinMain(HINSTANCE _hInstance, HINSTANCE _hPrevInstance, char*, int _nShowCmd)
{
    hInstance = _hInstance;
    hPrevInstance = _hPrevInstance;
    nShowCmd = _nShowCmd;

    return main(__argc, __argv);
}
#endif

void usage(int argc, char **argv)
{
    if (argc == 1)
    {
        std::cerr << argv[0] << " config_file.ini" << std::endl;
        exit(EXIT_FAILURE);
    }
}

std::string version()
{
    return (VERSION);
}
