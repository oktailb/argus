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

#include "configuration.h"

#ifdef WIN32
class SubProcessRunner
{
public:
    SubProcessRunner(const std::string& program, const std::string desktop)
        :
        program(program)
        , desktop(desktop)
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
        if( !CreateProcess( NULL, (LPSTR) program.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
            std::cerr << "CreateProcess " << program << " failed !" << std::endl;
        std::cerr << "Started." << std::endl;
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
};
#elif __linux__
class SubProcessRunner
{
public:
    SubProcessRunner(const std::string& program, const std::string desktop)
        : program(program)
    {
        std::cerr << "Will run " << program << std::endl;
    }

    void runSubProcess()
    {
        std::cerr << "Starting sub process ..." << std::endl;
        pid_t childPid = fork();

        if (childPid == 0) {
            // Child process
            execlp(program.c_str(), program.c_str(), nullptr);
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
    std::map<std::string, std::string> configuration = readConfiguration("config.ini");

    if (configuration.find("General/Child") != configuration.end())
    {
        input * capt = new input(configuration);
        capt->shoot();

        SubProcessRunner *subProcessRunner;
        int fps = std::stoi(configuration["General/fps"]);
        double delayMs = 1000.0f / fps;
        std::string desktop = "";
        bool videoSync = (configuration["General/videoSync"].compare("true") == 0);
#ifdef WIN32
        bool virtualDesktop = (configuration["General/virtualDesktop"].compare("true") == 0);
        if (virtualDesktop)
        {
            desktop = "Argus Desktop";
            createDedicatedDesktop(desktop.c_str(), NULL);
        }
#endif
        subProcessRunner = new SubProcessRunner(configuration["General/Child"], desktop);
        subProcessRunner->runSubProcess();

        auto start = std::chrono::high_resolution_clock::now();
        int counter = 0;
        while (subProcessRunner->active()) {
            auto begin = std::chrono::high_resolution_clock::now();

            capt->shoot();

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
            if (counter%1000 == 0) {
                auto step = std::chrono::high_resolution_clock::now();
                auto lap = std::chrono::duration_cast<std::chrono::milliseconds>(step - start);
                start = std::chrono::high_resolution_clock::now();
                std::cerr << "Capture FPS: " << 1000.0f * counter / lap.count()  << std::endl;
            }
        }
        delete subProcessRunner;
        delete capt;
    }
}

