#pragma once
#include <iostream>

#ifdef WIN32
#include <windows.h>
#endif

/**
 * @fn createSHM
 * @brief multiplatform method to initialize a SHM segment (SERVER)
 * @param key
 * @param size
 * @return a void pointer on the SHM start address
 */
#ifdef WIN32
LPVOID createSHM(const char *key, int size);
#elif __linux__
void* createSHM(const char *key, int size);
#endif

/**
 * @fn getSHM
 * @brief multiplatform method to get an existing SHM segment (CLIENT)
 * @param key
 * @param size
 * @return a void pointer on the SHM start address
 */
#ifdef WIN32
LPVOID getSHM(const char *key, int size);
#elif __linux__
void* getSHM(const char *key, int size);
#endif
