/**
 *
 * Simple and stupid picture from SHM displayer
 *
 */

#include "shm.h"
#include "desktop.h"
#include "types.h"

#include <windows.h>
#include <iostream>
#include <string>
#include <vector>

HANDLE hMapFile = nullptr;
t_argusExchange* header = nullptr;

/**
 * @brief updateImageFromSharedMemory
 * @param hdc
 * @param hBitmap
 * @brief Collect the shm picture data and produce am Operating syste display server compatible bitmap
 */
void updateImageFromSharedMemory(HDC hdc, HBITMAP hBitmap) {
    BITMAPINFO bmpInfo = {};
    bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfo.bmiHeader.biWidth = header->width;
    bmpInfo.bmiHeader.biHeight = -header->height;
    bmpInfo.bmiHeader.biPlanes = 1;
    bmpInfo.bmiHeader.biBitCount = 32;
    bmpInfo.bmiHeader.biCompression = BI_RGB;

    SetDIBitsToDevice(hdc, 0, 0, header->width, header->height, 0, 0, 0, header->height, (BYTE*)header + sizeof(t_argusExchange), &bmpInfo, DIB_RGB_COLORS);
}

int main() {
    std::string out0 = "prefix Argus SharedMemory";

    LPVOID shm = getSHM(out0.c_str(), sizeof(*header));
    header = (t_argusExchange*)shm;
    shm = getSHM(out0.c_str(), header->size + sizeof(*header));

    HWND hwnd = CreateWindowEx(0, "YourWindowClassName", "Image Viewer", WS_OVERLAPPEDWINDOW,
                               CW_USEDEFAULT, CW_USEDEFAULT, header->width, header->height, NULL, NULL, GetModuleHandle(NULL), NULL);
    if (hwnd == NULL) {
        std::cerr << "Failed to create window." << std::endl;
        return 1;
    }

    HDC hdc = GetDC(hwnd);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, header->width, header->height);

    while (true) {
        updateImageFromSharedMemory(hdc, hBitmap);
        Sleep(10);
    }

    DeleteObject(hBitmap);
    ReleaseDC(hwnd, hdc);
    UnmapViewOfFile(header);
    CloseHandle(hMapFile);

    return 0;
}
