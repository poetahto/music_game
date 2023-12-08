#ifndef MG_PLATFORM_WINDOWS_HPP
#define MG_PLATFORM_WINDOWS_HPP

#define WIN32_MEAN_AND_LEAN
#include "windows.h"

extern HINSTANCE g_instance;
extern HWND g_window;

char* wcharToChar(wchar_t* source);

#endif