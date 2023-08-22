#include <iostream>
#include "shm.h"

#ifdef WIN32

#include <windows.h>

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

#elif __linux__
#include <cstdlib>
#include <cstring>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void* getSHM(const char* key, size_t size) {
    void* shm;
    int fd = shm_open(key, O_RDONLY, 0);
    if (fd == -1) {
        std::cerr << __func__ << " Impossible d'ouvrir l'objet de mappage de fichier " << key << std::endl;
        throw;
    }

    shm = mmap(nullptr, size, PROT_READ, MAP_SHARED, fd, 0);
    if (shm == MAP_FAILED) {
        std::cerr << __func__ << " Impossible de créer la vue de l'objet de mappage de fichier " << key << " de taille " << size << std::endl;
                close(fd);
        throw;
    }

    close(fd);
    return shm;
}

void* createSHM(const char* key, size_t size) {
    void* shm;
    int fd = shm_open(key, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        std::cerr << __func__ << " Impossible de créer l'objet de mappage de fichier " << key << std::endl;
            throw;
    }

    if (ftruncate(fd, size) == -1) {
        std::cerr << __func__ << " Impossible de redimensionner l'objet de mappage de fichier " << key << std::endl;
        close(fd);
        throw;
    }

    shm = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shm == MAP_FAILED) {
        std::cerr << __func__ << " Impossible de créer la vue de l'objet de mappage de fichier " << key << std::endl;
                close(fd);
        throw;
    }

    return shm;
}
#endif
