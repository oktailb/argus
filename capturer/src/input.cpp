#include <input.hpp>
#include "shm.h"
#include "configuration.h"
#include "types.h"
#include <types.hpp>
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
#elif __linux__
#include <X11/X.h>
#endif

using namespace std;

std::vector<std::string> removeDuplicates(const std::vector<std::string>& inputVector)
{
    std::vector<std::string> resultVector;

    for (const std::string& str : inputVector) {
        if (std::find(resultVector.begin(), resultVector.end(), str) == resultVector.end()) {
            resultVector.push_back(str);
        }
    }
    std::sort(resultVector.begin(), resultVector.end());
    return resultVector;
}

#ifdef WIN32
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

LRESULT WINAPI input::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        exit(0);
        return(0);
    }
    return(DefWindowProc(hwnd, msg, wParam, lParam));
}

bool input::initDirectX(HWND hWndToCapture)
{
    pD3D = Direct3DCreate9(D3D_SDK_VERSION);

    d3dpp = {};
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_COPY;
    d3dpp.BackBufferWidth = width;
    d3dpp.BackBufferHeight = height;

    if (FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWndToCapture, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDevice))) {
        std::cerr << "CreateDevice Failed" << std::endl;
        return false;
    }
    return true;
}

bool input::captureDirectX(char * buffer)
{
    pDevice->CreateOffscreenPlainSurface(width, height, D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &pSurface, NULL);
    pDevice->GetFrontBufferData(0, pSurface);
    pSurface->LockRect(&lockedRect, NULL, D3DLOCK_NO_DIRTY_UPDATE|D3DLOCK_NOSYSLOCK|D3DLOCK_READONLY);
    for (int i = 0 ; i < height ; i++)
    {
        memcpy(buffer + i * width * 4 / 8, lockedRect.pBits + i * lockedRect.Pitch, width * 4 / 2);
    }
    pSurface->UnlockRect();

    return true;
}

void input::cleanupDirectX()
{
    if (pSurface) {
        pSurface->Release();
        pSurface = nullptr;
    }
    if (pDevice) {
        pDevice->Release();
        pDevice = nullptr;
    }
    if (pD3D) {
        pD3D->Release();
        pD3D = nullptr;
    }
}

bool input::initGDI(HWND hWndToCapture)
{
    hdcScreen = GetDC(nullptr);
    hdcWindow = GetDC(hWnd);
    hdcMem = CreateCompatibleDC(hdcScreen);
    hBitmap = CreateCompatibleBitmap(hdcScreen, width, height);
    hBitmapOld = (HBITMAP)SelectObject(hdcMem, hBitmap);

    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = width;
    bi.biHeight = -height; // Image est stockée à l'envers, donc la hauteur est négative
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
    hWindow = FindWindow(NULL,(LPCSTR)title.c_str());
    hScrDC = GetDC(hWindow);
    HDC hMemDC = CreateCompatibleDC(hScrDC);

    hBitmap = CreateCompatibleBitmap(hScrDC, width, height);
    hBitmapOld = (HBITMAP)SelectObject (hMemDC, hBitmap);
    BitBlt(hMemDC, 0, 0, width, height, hScrDC, 0, 0, SRCCOPY);

    hBitmap = (HBITMAP)SelectObject(hMemDC, hBitmapOld);

    BITMAP Bm;
    BITMAPINFO BitInfo;
    ZeroMemory(&BitInfo, sizeof(BITMAPINFO));
    ZeroMemory(&Bm, sizeof(BITMAP));
    BitInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    BitInfo.bmiHeader.biBitCount = 0;
    if(!::GetDIBits(hScrDC, hBitmap, 0, 0, NULL, &BitInfo, DIB_RGB_COLORS))
        return (NULL);
    Bm.bmHeight = BitInfo.bmiHeader.biHeight;
    Bm.bmWidth  = BitInfo.bmiHeader.biWidth;
    BitInfo.bmiHeader.biCompression = 0;

    if(!::GetDIBits(hdcWindow, hBitmap, 0, Bm.bmHeight, buffer, &BitInfo, DIB_RGB_COLORS))
    {
        return (NULL);
    }


    DeleteObject(hBitmapOld);
    DeleteObject(hBitmap);
    DeleteDC(hMemDC);
    ReleaseDC(hWindow, hScrDC);

    return true;
}

void input::cleanupGDI()
{
    SelectObject(hdcMem, hBitmapOld);
    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(nullptr, hdcScreen);
}

void input::configureWindow()
{
    hWnd = FindWindowA(nullptr, (LPCSTR)title.c_str());
    if (hWnd == nullptr) {
        std::map<std::string, HWND> list;
        GetOpenWindows(list);
        int i = 1;
        for (auto it = list.begin(); it != list.end() ; it++)
            cerr << i++ << ") " << it->first << endl;
        std::cerr << "wich one ? ";
        int answer = 0;
        cin >> answer;
        auto it = list.begin();
        for (i = 1; i < list.size() ; i++)
        {
            if (i == answer)
            {
                hWnd = it->second;
                title = it->first;
                configuration["General/title"] = it->first;
                saveConfiguration(configuration, "config.ini");
                break;
            }
            it++;
        }
        FindWindowW(nullptr, (LPCWSTR)title.c_str());
    }
    GetClientRect(hWnd, &rect);
    width = rect.right - rect.left;
    height = rect.bottom - rect.top;
}

input::input(std::map<std::string, std::string> &configuration)
    :
      configuration(configuration)
{
    bool res;

    title = configuration["General/title"];
    std::string method = configuration["General/method"];
    full = (title.compare("full") == 0);
    directX = (method.compare("directx") == 0);
    gdi = (method.compare("gdi") == 0);

    configureWindow();

    if (full)
    {
        width = GetSystemMetrics(SM_CXSCREEN);
        height = GetSystemMetrics(SM_CYSCREEN);

        hdcScreen = GetDC(nullptr);

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
    }
    else
    {
        if (gdi) initGDI(hWnd);
    }
    if (directX) initDirectX(hWnd);

    std::string out0 = configuration["General/Prefix"] + " Argus SharedMemory";

    const int SHM_SIZE = width*height*4 + sizeof(*header); // Taille de la mémoire partagée

    region = createSHM(out0.c_str(), SHM_SIZE);
    header = (t_argusExchange*)region;
    header->width = width;
    header->height = height;
    header->size = SHM_SIZE;
    header->inWrite = false;
}

input::~input()
{
    cleanupGDI();
    if (directX) cleanupDirectX();
}

void input::shoot()
{
    auto begin = std::chrono::high_resolution_clock::now();
    header->inWrite = true;
    if (full)
        captureFullScreen((char*)region + sizeof(*header), width, height);
    else
        captureSingleWindow((char*)region + sizeof(*header), width, height);
    header->inWrite = false;
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
    if (duration.count() < 1000/60)
    {
        Sleep(1000/60 - duration.count());
    }
}

bool input::captureFullScreen(char * buffer, int& width, int& height)
{
    if (!BitBlt(hdcMem, 0, 0, width, height, hdcScreen, 0, 0, SRCCOPY)) {
        DeleteDC(hdcMem);
        ReleaseDC(nullptr, hdcScreen);

        std::cerr << "BitBlt failed" << std::endl;
        return false;
    }

    GetDIBits(hdcMem, hBitmap, 0, height, buffer, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

    return true;
}

bool input::captureSingleWindow(char * buffer, int& width, int& height) {
    if (directX) captureDirectX(buffer);
    else if(gdi) captureGDI(buffer);

    return true;
}
#elif __linux__
void GetOpenWindows(std::map<std::string, Window*> & windowList)
{
    return;
}

bool input::initXSHM(Window* hWndToCapture)
{
    return true;
}

bool input::captureXSHM(char * buffer)
{
    return true;
}

void input::cleanupXSHM()
{
}

void input::configureWindow()
{
}

input::input(std::map<std::string, std::string> &configuration)
    :
    configuration(configuration)
{
    bool res;

    title = configuration["General/title"];
    std::string method = configuration["General/method"];
    full = (title.compare("full") == 0);
    directX = (method.compare("directx") == 0);
    gdi = (method.compare("gdi") == 0);

    configureWindow();

    if (full)
    {
    }
    else
    {
        initXSHM(window);
    }

    std::string out0 = configuration["General/Prefix"] + " Argus SharedMemory";

    const int SHM_SIZE = width*height*4 + sizeof(*header); // Taille de la mémoire partagée

    region = createSHM(out0.c_str(), SHM_SIZE);
    header = (t_argusExchange*)region;
    header->width = width;
    header->height = height;
    header->size = SHM_SIZE;
    header->inWrite = false;
}

input::~input()
{
    cleanupXSHM();
}

void input::shoot()
{
    auto begin = std::chrono::high_resolution_clock::now();
    header->inWrite = true;
    if (full)
        captureFullScreen((char*)region + sizeof(*header), width, height);
    else
        captureSingleWindow((char*)region + sizeof(*header), width, height);
    header->inWrite = false;
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    if (duration.count() < 1000/60)
    {
        usleep(1000000/60 - duration.count());
    }
}

bool input::captureFullScreen(char * buffer, int& width, int& height)
{
    return true;
}

bool input::captureSingleWindow(char * buffer, int& width, int& height) {
    captureXSHM(buffer);
    return true;
}
#endif
