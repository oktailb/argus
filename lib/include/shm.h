#pragma once
#include <iostream>

#ifdef WIN32
LPVOID createSHM(const char *key, int size);
LPVOID getSHM(const char *key, int size);
#elif __linux__
void* createSHM(const char *key, int size);
void* getSHM(const char *key, int size);
#endif
