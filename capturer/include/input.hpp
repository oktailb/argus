#pragma once

#include <ddraw.h>
#include <windows.h>
#pragma comment(lib, "d3d9.lib")
#include <d3d9.h>
#pragma comment(lib, "d3dx9.lib")
#include <d3dx9.h>
//#include<d3dx9.h>
#include <iostream>
#include <types.hpp>
#include <D3dx9tex.h>
#include <vector>
#include <map>
#include <string>
#include "types.h"

class input
{
public:
    input(std::map<std::string, std::string> &configuration);
    ~input();

    std::map<std::string, std::string>                  &configuration;

	HDC													hScrDC;
	HWND												hWindow;
	int													width;
	int													height;
    std::string												title;
	bool												full;
    bool                                                directX;
    bool                                                gdi;

    LPVOID                                              region;
    bool                                                captureSingleWindow(char * buffer, int& width, int& height);
    bool                                                captureFullScreen(char * buffer, int& width, int& height);

	static LRESULT WINAPI								WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void												shoot();
    void                                                configureWindow();

#ifdef WIN32
    HWND hWnd;
    BITMAPINFOHEADER bi;
    RECT rect;
    HDC hdcScreen;
    HDC hdcWindow;
    HDC hdcMem;
    HBITMAP hBitmap;
    HBITMAP hBitmapOld;
    bool initGDI(HWND hWndToCapture);
    bool captureGDI(char *buffer);
    void cleanupGDI();

    IDirect3D9*             pD3D;
    IDirect3DDevice9*       pDevice;
    IDirect3DSurface9*      pSurface;
    D3DPRESENT_PARAMETERS   d3dpp;
    D3DLOCKED_RECT          lockedRect;

    int                     nb_adapters;
    int                     nb_screens;

    bool initDirectX(HWND hWndToCapture);
    bool captureDirectX(char *buffer);
    void cleanupDirectX();
#endif
    t_argusExchange     *header;
};
