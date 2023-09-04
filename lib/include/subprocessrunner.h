#pragma once
#ifdef WIN32
#include <windows.h>
#include "desktop.h"
#elif __linux__
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

#include <iostream>
#include <string>
#include <map>
#include <string.h>

class SubProcessRunner
{
public:
    SubProcessRunner(const std::string& program, const std::string& desktop, int argc, char** argv);
    ~SubProcessRunner();
    void runSubProcess();
    bool active();

private:
    std::string         desktop;
    std::string         program;
    int                 argc;
    char**              argv;
#ifdef WIN32
    STARTUPINFO         si;
    PROCESS_INFORMATION pi;
    DWORD               status;
#elif __linux__
    pid_t               childPid;
#endif
};
