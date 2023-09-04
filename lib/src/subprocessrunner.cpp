#include "subprocessrunner.h"

SubProcessRunner::SubProcessRunner(const std::string& program, const std::string& desktop, int argc, char** argv)
    : program(program)
    , argc(argc)
    , argv(argv)
    , desktop(desktop)
{
    std::cerr << "Will run " << program << std::endl;
}

SubProcessRunner::~SubProcessRunner()
{
#ifdef WIN32
    if (desktop.length() != 0)
        restoreOriginalDesktop();
#endif
}

#ifdef WIN32
void SubProcessRunner::runSubProcess()
{
    if (desktop.length() != 0)
        createDedicatedDesktop(desktop.c_str(), NULL);

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

bool SubProcessRunner::active()
{
    GetExitCodeProcess(pi.hProcess, &status);
    return (status == STILL_ACTIVE);
}

#elif __linux__
void SubProcessRunner::runSubProcess()
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

bool SubProcessRunner::active()
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

void sigchldHandler(int signo)
{
    // This handler does nothing; it's only used to interrupt sleep
}
#endif
