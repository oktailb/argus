#pragma once

#ifdef WIN32
#include <windows.h>

void restoreOriginalDesktop();
void createDedicatedDesktop(const char * name, HWND thread);
void switchToOriginalDesktop();
void switchToNewDesktop();

#elif __linux__
//useless ?
#endif
