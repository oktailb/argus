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

bool input::initGDI(HWND hWndToCapture)
{
    hdcScreen = GetDC(nullptr);
    hdcWindow = GetDC(hWnd);
    hdcMem = CreateCompatibleDC(hdcScreen);
    hBitmap = CreateCompatibleBitmap(hdcScreen, width, height);
    hBitmapOld = (HBITMAP)SelectObject(hdcMem, hBitmap);

    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = width;
    bi.biHeight = -height;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;
    return true;
}

bool input::captureGDI(char * buffer)
{
    hScrDC = GetDC(hWnd);
    if (!hScrDC)
        exit(EXIT_FAILURE);

    HDC hMemDC = CreateCompatibleDC(hScrDC);
    if (!hMemDC)
        exit(EXIT_FAILURE);

    hBitmap = CreateCompatibleBitmap(hScrDC, width, height);
    if (!hBitmap)
        exit(EXIT_FAILURE);

    HBITMAP hBitmapOld = (HBITMAP)SelectObject(hMemDC, hBitmap);
    if (!hBitmapOld)
        exit(EXIT_FAILURE);

    if (!BitBlt(hMemDC, 0, 0, width, height, hScrDC, 0, 0, SRCCOPY))
        exit(EXIT_FAILURE);

    BITMAP Bm;
    BITMAPINFO BitInfo;
    ZeroMemory(&BitInfo, sizeof(BITMAPINFO));
    ZeroMemory(&Bm, sizeof(BITMAP));
    BitInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    BitInfo.bmiHeader.biBitCount = 0; // Exemple : 24 bits par pixel pour RVB
    if (!GetDIBits(hMemDC, hBitmap, 0, 0, NULL, &BitInfo, DIB_RGB_COLORS))
        exit(EXIT_FAILURE);
    Bm.bmHeight = BitInfo.bmiHeader.biHeight;
    Bm.bmWidth = BitInfo.bmiHeader.biWidth;
    BitInfo.bmiHeader.biCompression = 0;

    if (!GetDIBits(hMemDC, hBitmap, 0, Bm.bmHeight, buffer, &BitInfo, DIB_RGB_COLORS))
        exit(EXIT_FAILURE);

    DeleteObject(hBitmapOld);
    DeleteObject(hBitmap);
    DeleteDC(hMemDC);
    ReleaseDC(hWnd, hScrDC);

    return true;
}

void input::cleanupGDI()
{
    SelectObject(hdcMem, hBitmapOld);
    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(nullptr, hdcScreen);
}

#endif
