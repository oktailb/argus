#pragma once
#include <windows.h>

void restoreOriginalDesktop();
void createDedicatedDesktop(const char * name, HWND thread);
void switchToOriginalDesktop();
void switchToNewDesktop();
