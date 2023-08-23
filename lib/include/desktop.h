#pragma once
/**
 * @file desktop.h
 * @brief Collection of methods to easyly manage virtual desktops on windows (API exists long time before the feature be available for users)
 */

#ifdef WIN32

/**
 * @fn createDedicatedDesktop
 * @param name
 * @param thread
 * @brief Configure a new (if necessary) virtual desktop defined by his name and attach a specific thread to it.
 */
void createDedicatedDesktop(const char * name, HWND thread);

/**
 * @fn restoreOriginalDesktop
 * @brief destroy the previously created virtual desktop ans switch back to the original one
 */
void restoreOriginalDesktop();

/**
 * @fn switchToOriginalDesktop
 * @brief switch back to the original desktop without destroying the virtual one
 */
void switchToOriginalDesktop();

/**
 * @fn switchToNewDesktop
 * @brief switch fron original to virtual desktop
 */
void switchToNewDesktop();

#elif __linux__
//useless ?
#endif
