#ifndef MG_PLATFORM_WINDOWS_HPP
#define MG_PLATFORM_WINDOWS_HPP

#include "platform.hpp"
#include "types.hpp"

#define WIN32_MEAN_AND_LEAN
#include "windows.h"

extern HINSTANCE g_instance;
extern HWND g_window;

char* wcharToChar(wchar_t* source);
void printLastError();

void audioInit();
void audioFree();

void rendererInit();
void rendererFree();
void rendererUpdateResolution(u32 width, u32 height);
bool rendererProcessInput(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

#endif