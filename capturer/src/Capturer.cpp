#include <input.hpp>
#include <windows.h>
#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include "configuration.h"
#include "desktop.h"

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

int main(int argc, char *argv[])
{
    std::map<std::string, std::string> configuration = readConfiguration("config.ini");
    bool virtualDesktop = (configuration["General/virtualDesktop"].compare("true") == 0);

    if (configuration.find("General/Child") != configuration.end())
    {
        input * capt = new input(configuration);
        capt->shoot();

        SubProcessRunner *subProcessRunner;
        std::string desktop = "";
        if (virtualDesktop)
        {
            desktop = "Argus Desktop";
            createDedicatedDesktop(desktop.c_str(), NULL);
        }
        subProcessRunner = new SubProcessRunner(configuration["General/Child"], desktop);
        subProcessRunner->runSubProcess();

        while (subProcessRunner->active())
            capt->shoot();
        delete capt;
    }
}
