#ifdef WIN32
#include <windows.h>
#include "desktop.h"
#include <iostream>

static HDESK    hOriginalThread;
static HDESK    hOriginalInput;
static HDESK    hNewDesktop;
static HWND     hOriginalDesktopWindow;
static HDC      hOriginalDC;

void switchToOriginalDesktop()
{
    SwitchDesktop(hOriginalInput);
}

void switchToNewDesktop()
{
    SwitchDesktop(hNewDesktop);
}

void restoreOriginalDesktop()
{
    /* Revenir au bureau d'origine */
    SwitchDesktop(hOriginalInput);
    SetThreadDesktop(hOriginalThread);

    /* Fermer le bureau créé dans goNextDesktop() s'il existe encore */
    if (hNewDesktop != nullptr)
    {
        CloseDesktop(hNewDesktop);
        hNewDesktop = nullptr;
    }
}

void createDedicatedDesktop(const char * name, HWND thread)
{
    std::cerr << "Creating virtual desktop ..." << std::endl;
    /* Backup du bureau (desktop) d'origine */
    hOriginalThread = GetThreadDesktop(GetCurrentThreadId());

    /* Ouverture des E/S du bureau courant */
    hOriginalInput = OpenInputDesktop(0, FALSE, DESKTOP_SWITCHDESKTOP);

    /* Création d'un nouveau bureau (vérifier si le bureau existe déjà) */
    hNewDesktop = OpenDesktop(name, 0, FALSE, GENERIC_ALL);
    if (hNewDesktop == nullptr)
    {
        hNewDesktop = CreateDesktop(name, NULL, NULL, 0, DELETE | READ_CONTROL | WRITE_DAC | WRITE_OWNER | GENERIC_ALL, NULL);
    }

    /* Attribution du processus au nouveau bureau */
    SetThreadDesktop(hNewDesktop);

    /* Basculer vers le nouveau bureau */
    SwitchDesktop(hNewDesktop);
}
#elif __linux__
#endif
