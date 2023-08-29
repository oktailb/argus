#include <input.hpp>
#include "shm.h"
#include "configuration.h"
#include "types.h"
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <algorithm>
#include <chrono>

#ifdef WIN32
#include <windows.h>
#include <DXGI.h>
#include <d3dx9tex.h>
#include <windows.h>
#pragma comment(lib, "d3d9.lib")
#include <d3d9.h>
#include <dwmapi.h>
#define STRICT  1
#include <windows.h>
#include <d3d11.h>
#include <d3d11sdklayers.h>
#include <dxgi.h>
#include <d3dcommon.h>
#include <D3Dcompiler.h>
#include <Windows.h>
#include <psapi.h>		// NT only!
#pragma comment(lib, "psapi")	// NT only!

using namespace std;

#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#define __WFILE__ WIDEN(__FILE__)
#define HRCHECK(__expr) {hr=(__expr);if(FAILED(hr)){wprintf(L"FAILURE 0x%08X (%i)\n\tline: %u file: '%s'\n\texpr: '" WIDEN(#__expr) L"'\n",hr, hr, __LINE__,__WFILE__);return false;}}
#define RELEASE(__p) {if(__p!=nullptr){__p->Release();__p=nullptr;}}

bool input::initDirectX(HWND hWndToCapture)
{
    UINT adapter = D3DADAPTER_DEFAULT;
    HRESULT hr = S_OK;
    D3DDISPLAYMODE mode;

    d3dpp = { 0 };
    pD3D = Direct3DCreate9(D3D_SDK_VERSION);
    HRCHECK(pD3D->GetAdapterDisplayMode(adapter, &mode));

    d3dpp.Windowed = TRUE;
    d3dpp.BackBufferCount = 1;
    d3dpp.BackBufferHeight = height;
    d3dpp.BackBufferWidth = width;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow = hWndToCapture;

    HRCHECK(pD3D->CreateDevice(adapter, D3DDEVTYPE_HAL, NULL, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDevice));
    HRCHECK(pDevice->CreateOffscreenPlainSurface(width, height, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &pSurface, nullptr));

    return true;
}

bool input::captureDirectX(char * buffer)
{
    HRESULT hr = S_OK;

    HRCHECK(pSurface->LockRect(&lockedRect, NULL, 0));
    memcpy(buffer, lockedRect.pBits, lockedRect.Pitch * height);
    HRCHECK(pSurface->UnlockRect());

    return true;
}

void input::cleanupDirectX()
{
    RELEASE(pSurface);
    RELEASE(pDevice);
    RELEASE(pD3D);
}

#endif
