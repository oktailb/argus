//#include <QtWidgets/QApplication>
//#include <QtWidgets/QLabel>
//#include <QtGui/QImage>
//#include <QtGui/QPixmap>
//#include <QtCore/QTimer>
//#include <QDebug>
//#include <iostream>

//#include <windows.h>
//#include "shm.h"
//#include "desktop.h"
//#include "types.h"

//int width = 1920;
//int height = 1080;
//t_argusExchange *header;

//// Fonction pour mettre à jour l'image à partir de la mémoire partagée
//void updateImageFromSharedMemory(QImage& image, LPVOID sharedMemory, QLabel &label) {

//        // Copier les données depuis la mémoire partagée vers l'image
//        header = (t_argusExchange*) sharedMemory;
//        image = QImage((const uchar*)sharedMemory + sizeof(*header), header->width, header->height, QImage::Format_RGB32);

//        // Mettre à jour l'affichage
//        label.setPixmap(QPixmap::fromImage(image));
//}

//int main(int argc, char *argv[]) {
//    //createDedicatedDesktop("Argus Desktop", NULL);
//    QApplication app(argc, argv);

//    std::string out0 = "prefix Argus SharedMemory";

//    LPVOID shm;
//    shm = getSHM(out0.c_str(), sizeof(*header));
//    header = (t_argusExchange*) shm;
//    shm = getSHM(out0.c_str(), header->size);

//    // Créer l'image et l'afficher dans une fenêtre
//    QImage image(width, height, QImage::Format_RGB32);
//    QLabel label;

//    label.setPixmap(QPixmap::fromImage(image));
//    label.show();

//    // Créer un QTimer pour mettre à jour l'image à intervalles réguliers
//    QTimer timer;
//    QObject::connect(&timer, &QTimer::timeout, [&]() {
//        updateImageFromSharedMemory(image, shm, label);
//    });

//    // Définir l'intervalle de mise à jour en millisecondes (par exemple 100 ms)
//    const int updateInterval = 10;
//    timer.start(updateInterval);

//    return app.exec();
//}

#include "shm.h"
#include "desktop.h"
#include "types.h"

#include <windows.h>
#include <iostream>
#include <string>
#include <vector>

int width = 1920;
int height = 1080;
HANDLE hMapFile = nullptr;
t_argusExchange* header = nullptr;

// Fonction pour mettre à jour l'image à partir de la mémoire partagée
void updateImageFromSharedMemory(HDC hdc, HBITMAP hBitmap) {
    BITMAPINFO bmpInfo = {};
    bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfo.bmiHeader.biWidth = width;
    bmpInfo.bmiHeader.biHeight = -height; // Négatif pour inverser l'orientation Y
    bmpInfo.bmiHeader.biPlanes = 1;
    bmpInfo.bmiHeader.biBitCount = 32; // 32 bits par pixel (RGBA)
    bmpInfo.bmiHeader.biCompression = BI_RGB;

    // Copier les données depuis la mémoire partagée vers le bitmap
    SetDIBitsToDevice(hdc, 0, 0, header->width, header->height, 0, 0, 0, header->height, (BYTE*)header + sizeof(t_argusExchange), &bmpInfo, DIB_RGB_COLORS);

    // Mettre à jour l'affichage
    // ...
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

    // ... Initialisation de la fenêtre GDI ...

    HDC hdc = GetDC(hwnd); // Récupérer le contexte de périphérique
    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, width, height);

    // Boucle de mise à jour de l'image à partir de la mémoire partagée
    while (true) {
        updateImageFromSharedMemory(hdc, hBitmap);

        // ... Mettre à jour l'affichage GDI ...

        Sleep(10); // Attendre avant la prochaine mise à jour
    }

    // Nettoyage
    DeleteObject(hBitmap);
    ReleaseDC(hwnd, hdc); // Libérer le contexte de périphérique
    UnmapViewOfFile(header);
    CloseHandle(hMapFile);

    return 0;
}
