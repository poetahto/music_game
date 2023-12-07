 #ifdef _WIN32

#define WIN32_MEAN_AND_LEAN
#include "windows.h"
#include "platform.hpp"
#include <stdio.h>

static HINSTANCE s_instance {};
static HWND s_window {};
static bool s_wantsToQuit {false};

static LRESULT WindowEventHandler(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

void Platform::init() {
    s_instance = GetModuleHandle(nullptr);

    WNDCLASS windowClass {};
    windowClass.lpszClassName = "MusicGame";
    windowClass.hInstance = s_instance;
    windowClass.lpfnWndProc = WindowEventHandler;
    // windowClass.hIcon; // todo: at some point, add this
    RegisterClass(&windowClass);

    s_window = CreateWindow(
        windowClass.lpszClassName, 
        "Music Game", // title
        WS_OVERLAPPEDWINDOW, 
        0, 0, 800, 600, // x, y, width, height
        nullptr,
        nullptr, 
        s_instance, 
        nullptr 
    );

    ShowWindow(s_window, SW_NORMAL);
}

void Platform::free() {
    DestroyWindow(s_window);
}

void Platform::sleep(u32 duration) {
    Sleep(duration);
}

bool Platform::wantsToQuit() {
    return s_wantsToQuit;
}

void Platform::handleEvents() {
    MSG msg {};
    while (PeekMessage(&msg, s_window, 0, 0, true) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

static LRESULT WindowEventHandler(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_DESTROY: {
            PostQuitMessage(0);
            s_wantsToQuit = true;
            return 0;
        }
    }
    return DefWindowProc(window, message, wParam, lParam);
}

 #endif // _WIN32