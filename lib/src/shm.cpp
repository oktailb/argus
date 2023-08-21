#include <windows.h>
#include <iostream>
#include "shm.h"

LPVOID getSHM(const char *key, int size)
{
    LPVOID shm;
    // Ouvrir l'objet de mappage de fichier existant
    HANDLE hFileMap = OpenFileMapping(FILE_MAP_READ, FALSE, key);
    if (hFileMap == nullptr) {
        std::cerr << __func__ << " Impossible d'ouvrir l'objet de mappage de fichier " << key << ". Code d'erreur : " << GetLastError() << std::endl;
        throw;
    }

    // Créer une vue de l'objet de mappage de fichier dans l'espace d'adressage virtuel du processus
    shm = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, size);
    if (shm == nullptr) {
        std::cerr << __func__ << " Impossible de créer la vue de l'objet de mappage de fichier " << key << " de taille " << size << ". Code d'erreur : " << GetLastError() << std::endl;
        CloseHandle(hFileMap);
        throw;
    }
    return shm;
}

LPVOID createSHM(const char * key, int size)
{
    LPVOID shm;

    // Créer un objet de mappage de fichier
    HANDLE hFileMap = CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, size, key);
    if (hFileMap == nullptr) {
        std::cerr << __func__ << " Impossible de créer l'objet de mappage de fichier de taille " << size << ". Code d'erreur : " << GetLastError() << std::endl;
        throw;
    }

    // Créer une vue de l'objet de mappage de fichier dans l'espace d'adressage virtuel du processus
    shm = MapViewOfFile(hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, size);
    if (shm == nullptr) {
        std::cerr << __func__ << " Impossible de créer la vue de l'objet de mappage de fichier. Code d'erreur : " << GetLastError() << std::endl;
        CloseHandle(hFileMap);
        throw;
    }
    std::cerr << "shm " << key << " created" << std::endl;
    return shm;
}
