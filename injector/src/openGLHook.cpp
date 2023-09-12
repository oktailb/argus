#include <Windows.h>
#include <gl/GL.h>
#include "minhook/include/MinHook.h"

typedef BOOL(__stdcall* wglSwapBuffers)(_In_ HDC hdc);
typedef uintptr_t PTR;
extern PTR SwapBuffersAddr;
extern wglSwapBuffers oSwapBuffers;

namespace PHook
{
void Init();
void Shutdown();
void SetupOrtho();
void Restore();
BOOL __stdcall hkSwapBuffers(_In_ HDC hdc);
}


#pragma comment(lib, "OpenGL32.lib")

void PHook::Init()
{
    SwapBuffersAddr = NULL;
    while (SwapBuffersAddr == NULL)
        SwapBuffersAddr = (PTR)(GetProcAddress(GetModuleHandle("opengl32.dll"), "wglSwapBuffers"));
    MH_Initialize();
    MH_CreateHook((LPVOID)SwapBuffersAddr, (LPVOID)hkSwapBuffers, (LPVOID*)& oSwapBuffers);
    MH_EnableHook((LPVOID)SwapBuffersAddr);
}

void PHook::Shutdown()
{
    MH_DisableHook((LPVOID)SwapBuffersAddr);
    MH_RemoveHook((LPVOID)SwapBuffersAddr);
    MH_Uninitialize();
}

void PHook::SetupOrtho()
{
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glViewport(0, 0, viewport[2], viewport[3]);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, viewport[2], viewport[3], 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
}

void PHook::Restore()
{
    glEnable(GL_DEPTH_TEST);
}

PTR SwapBuffersAddr;
wglSwapBuffers oSwapBuffers;

bool init = false;
BOOL __stdcall PHook::hkSwapBuffers(_In_ HDC hdc)
{
    if (!init)
    {
        MessageBox(NULL, "Success!", "SwapBuffers Hook", MB_OK);
        init = true;
    }
    PHook::SetupOrtho();
    //Draw Here
    PHook::Restore();
    return oSwapBuffers(hdc);
}

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved)
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hMod);
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)PHook::Init, hMod, 0, nullptr);
        break;
    case DLL_PROCESS_DETACH:
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)PHook::Shutdown, hMod, 0, nullptr);
        break;
    }
    return TRUE;
}
