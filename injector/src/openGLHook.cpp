#include <Windows.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <string>

#include "types.h"

#pragma data_seg(".shared")
extern int width;
extern int height;
extern std::string prefix;
extern LPVOID region;
extern t_argusExchange* header;
extern LPVOID shm;
#pragma data_seg()

typedef BOOL (APIENTRY* PFNSWAPBUFFERS)(HDC);
PFNSWAPBUFFERS pfnSwapBuffers = NULL;

BOOL APIENTRY MySwapBuffers(HDC hdc) {
    glReadPixels(0, 0, header->width, header->height, GL_RGBA, GL_UNSIGNED_BYTE, (char*)shm + sizeof(header));
    exit(0);
    return pfnSwapBuffers(hdc);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
#ifdef WIN32
        pfnSwapBuffers = (PFNSWAPBUFFERS)wglGetProcAddress("SwapBuffers");
#endif

        if (pfnSwapBuffers == NULL) {
            return FALSE;
        }

        DWORD oldProtect;
        SIZE_T codeSize = (SIZE_T)&MySwapBuffers - (SIZE_T)pfnSwapBuffers;
        VirtualProtect((PVOID)pfnSwapBuffers, codeSize, PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy((PVOID)pfnSwapBuffers, (PVOID)MySwapBuffers, codeSize);
        VirtualProtect((PVOID)pfnSwapBuffers, codeSize, oldProtect, NULL);
    }
    else if (ul_reason_for_call == DLL_PROCESS_DETACH) {
        // Détachez le hook lorsque la DLL est déchargée
        // Il est important de restaurer l'état d'origine ici
    }
    return TRUE;
}
