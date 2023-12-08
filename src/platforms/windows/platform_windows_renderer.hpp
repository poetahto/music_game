#ifndef MG_PLATFORM_WINDOWS_RENDERER_HPP
#define MG_PLATFORM_WINDOWS_RENDERER_HPP

#include "types.hpp"

void rendererInit();
void rendererFree();
void rendererUpdateResolution(u32 width, u32 height);
bool rendererProcessInput(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

#endif