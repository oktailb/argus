#ifdef WIN32
#include <windows.h>
#include "desktop.h"
#elif __linux__
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

#include <input.hpp>
#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <chrono>

#include "argus.h"
#include "configuration.h"

#ifdef WIN32
class SubProcessRunner
{
public:
    SubProcessRunner(const std::string& program, const std::string& desktop, int argc, char** argv)
        : program(program)
        , desktop(desktop)
        , argc(argc)
        , argv(argv)
    {
        std::cerr << "Will run " << program << std::endl;
    }

    void runSubProcess()
    {
        std::cerr << "Starting sub process ..." << std::endl;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        if (desktop.length() != 0)
            si.lpDesktop = (char*)desktop.c_str();
        ZeroMemory(&pi, sizeof(pi));

        std::string cmdLine = program;
        for (int i = 1; i < argc; ++i)
        {
            cmdLine += " ";
            cmdLine += argv[i];
        }

        if (!CreateProcess(NULL, (LPSTR)cmdLine.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
            std::cerr << "CreateProcess " << program << " failed !" << std::endl;
        std::cerr << cmdLine << " started." << std::endl;
    }

    bool active()
    {
        GetExitCodeProcess(pi.hProcess, &status);
        return (status == STILL_ACTIVE);
    }

private:
    STARTUPINFO         si;
    PROCESS_INFORMATION pi;
    DWORD               status;
    std::string         program;
    std::string         desktop;
    int                 argc;
    char**              argv;
};
#elif __linux__
class SubProcessRunner
{
public:
    SubProcessRunner(const std::string& program, int argc, char** argv)
        : program(program)
        , argc(argc)
        , argv(argv)
    {
        std::cerr << "Will run " << program << std::endl;
    }

    void runSubProcess()
    {
        std::cerr << "Starting sub process ..." << std::endl;
        pid_t childPid = fork();

        if (childPid == 0) {
            // Child process
            char** newArgv = new char*[argc + 2];
            newArgv[0] = strdup(program.c_str());
            for (int i = 1; i <= argc; ++i) {
                newArgv[i] = strdup(argv[i - 1]);
            }
            newArgv[argc + 1] = nullptr;

            execvp(program.c_str(), newArgv);
            std::cerr << "Error running " << program << std::endl;
            _exit(1);
        }
        else if (childPid > 0) {
            // Parent process
            this->childPid = childPid;
        }
        else {
            // Error forking
            std::cerr << "Error forking process." << std::endl;
        }
    }

    bool active()
    {
        int status;
        pid_t result = waitpid(childPid, &status, WNOHANG);
        if (result == -1) {
            // Error occurred while waiting
            return false;
        }
        else if (result == 0) {
            // Child process still active
            return true;
        }
        else {
            // Child process terminated
            return false;
        }
    }

private:
    std::string program;
    pid_t childPid;
};

void sigchldHandler(int signo)
{
    // This handler does nothing; it's only used to interrupt sleep
}
#endif


int main(int argc, char *argv[])
{
    usage(argc, argv);
    std::map<std::string, std::string> configuration = readConfiguration(argv[1]);

    if (configuration.find("General/Child") != configuration.end())
    {
        input * capt = new input(argv[1]);

        SubProcessRunner *subProcessRunner;
        std::string desktop = "";
#ifdef WIN32
        bool virtualDesktop = (configuration["General/virtualDesktop"].compare("true") == 0);
        if (virtualDesktop)
        {
            desktop = "Argus Desktop";
            createDedicatedDesktop(desktop.c_str(), NULL);
        }
#endif
        subProcessRunner = new SubProcessRunner(configuration["General/Child"], desktop, argc, argv);
        subProcessRunner->runSubProcess();

        while (subProcessRunner->active()) {
            capt->shoot();
        }
        delete subProcessRunner;
        delete capt;
    }
}

