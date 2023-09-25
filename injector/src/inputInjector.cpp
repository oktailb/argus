#include <iostream>
#include <Windows.h>
#include <tlhelp32.h>

#include <map>
#include <vector>
#include <iostream>
#include "shm.h"
#include "types.h"
#include "argus.h"
#include "configuration.h"
#include "subprocessrunner.h"

#pragma data_seg(".shared")
int width;
int height;
std::string prefix;
LPVOID region;
t_argusExchange* header;
LPVOID shm;
#pragma data_seg()

void GetOpenWindows(std::map<std::string, HWND> & windowList)
{
    // Enumerate through all top-level windows
    EnumWindows([](HWND hwnd, LPARAM lParam)
                {
                    std::map<std::string, HWND>& windowList = *reinterpret_cast<std::map<std::string, HWND>*>(lParam);

                    const int titleLength = GetWindowTextLength(hwnd);
                    if (titleLength > 0)
                    {
                        std::vector<char> titleBuffer(titleLength + 1);
                        GetWindowTextA(hwnd, titleBuffer.data(), titleLength + 1);
                        HWND CheckhWnd = FindWindowA(nullptr, (LPSTR)titleBuffer.data());
                        if (CheckhWnd != nullptr)
                            windowList[titleBuffer.data()] = hwnd;
                    }

                    return TRUE;
                }, reinterpret_cast<LPARAM>(&windowList));
    return;
}

int main(int argc, char **argv) {
    const char* filename = argv[1];
    const char* dllPath = argv[2];
    const char* targetProcessPath = argv[3];

    std::map<std::string, std::string>                  configuration;
    configuration = readConfiguration(filename);
    std::string out0 = configuration["General/Prefix"] + " Argus SharedMemory";
    HWND												hWnd;
    RECT                                                rect;
    std::string title = configuration["General/title"];
    hWnd = FindWindowA(nullptr, (LPCSTR)title.c_str());
    if (hWnd == nullptr) {
        std::map<std::string, HWND> list;
        GetOpenWindows(list);
        int i = 1;
        for (auto it = list.begin(); it != list.end() ; it++)
            std::cerr << i++ << ") " << it->first << std::endl;
        std::cerr << "wich one ? ";
        int answer = 0;
        std::cin >> answer;
        auto it = list.begin();
        for (i = 1; i < list.size() ; i++)
        {
            if (i == answer)
            {
                hWnd = it->second;
                title = it->first;
                configuration["General/title"] = it->first;
                saveConfiguration(configuration, filename);
                break;
            }
            it++;
        }
        FindWindowW(nullptr, (LPCWSTR)title.c_str());
    }
    GetClientRect(hWnd, &rect);
    width = rect.right - rect.left;
    height = rect.bottom - rect.top;
    prefix = configuration["General/Prefix"] + " Argus SharedMemory";

    std::cerr << "window '" << title << "' grabbed " << width << "x" << height << " for shm prefix " << prefix << std::endl;

    const int SHM_SIZE = 2 * (width*height*4) + sizeof(*header);
    region = createSHM(prefix.c_str(), SHM_SIZE);
    std::cerr << "SHM created on " << region << std::endl;
    header = (t_argusExchange*)region;
    header->width = width;
    header->height = height;
    header->size = width*height*4;
    header->firstBufferInWrite = false;

    header = (t_argusExchange*)shm;


    DWORD processId = 0;
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnapshot == INVALID_HANDLE_VALUE) {
        std::cerr << "Erreur lors de la création du snapshot du processus." << std::endl;
            return 1;
    }

    if (Process32First(hSnapshot, &pe32)) {
        do {
            if (_stricmp(pe32.szExeFile, targetProcessPath) == 0) {
                processId = pe32.th32ProcessID;
                break;
            }
        } while (Process32Next(hSnapshot, &pe32));
    }

    CloseHandle(hSnapshot);

    if (processId == 0) {
        std::cerr << "Processus cible introuvable." << std::endl;
        return 1;
    }

    HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, processId);

    if (hProcess == NULL) {
        std::cerr << "Impossible d'ouvrir le processus cible." << std::endl;
        return 1;
    }

    LPVOID remoteString = VirtualAllocEx(hProcess, NULL, strlen(dllPath) + 1, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    if (remoteString == NULL) {
        std::cerr << "Impossible d'allouer de la mémoire dans le processus cible." << std::endl;
                CloseHandle(hProcess);
        return 1;
    }

    if (!WriteProcessMemory(hProcess, remoteString, dllPath, strlen(dllPath) + 1, NULL)) {
        std::cerr << "Impossible d'écrire dans le processus cible." << std::endl;
                VirtualFreeEx(hProcess, remoteString, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return 1;
    }

    FARPROC loadLibraryAddr = GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");

    if (loadLibraryAddr == NULL) {
        std::cerr << "Impossible d'obtenir l'adresse de LoadLibraryA." << std::endl;
        VirtualFreeEx(hProcess, remoteString, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return 1;
    }

    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)loadLibraryAddr, remoteString, 0, NULL);

    if (hThread == NULL) {
        std::cerr << "Impossible de créer un thread dans le processus cible." << std::endl;
                VirtualFreeEx(hProcess, remoteString, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return 1;
    }

    WaitForSingleObject(hThread, INFINITE);

    CloseHandle(hThread);
    VirtualFreeEx(hProcess, remoteString, 0, MEM_RELEASE);
    CloseHandle(hProcess);

    std::cout << "DLL injected." << std::endl;
            Sleep(1000);

    SubProcessRunner *subProcessRunner;
    std::string desktop = "";

    subProcessRunner = new SubProcessRunner(configuration["General/Child"], desktop, argc, argv);
    subProcessRunner->runSubProcess();

    while (subProcessRunner->active()) {
        Sleep(1000);
    }
    delete subProcessRunner;
    return 0;
}
