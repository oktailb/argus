#pragma once
#include <windows.h>
#include <iostream>

LPVOID createSHM(const char *key, int size);
LPVOID getSHM(const char *key, int size);
