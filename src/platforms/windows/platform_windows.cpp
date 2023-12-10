#ifdef _WIN32

#include "platforms/platform_windows.hpp"
#include <stdio.h>

static LRESULT WindowEventHandler(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

HINSTANCE g_instance {};
HWND g_window {};

static int s_width {800};
static int s_height {600};
static bool s_wantsToQuit {false};
static WNDCLASS s_windowClass {};
static bool s_resizeDirty {};

void Platform::init() {
    g_instance = GetModuleHandle(nullptr);
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    s_windowClass.lpszClassName = "MusicGame";
    s_windowClass.hInstance = g_instance;
    s_windowClass.lpfnWndProc = WindowEventHandler;
    // windowClass.hIcon; // todo: at some point, add this
    RegisterClass(&s_windowClass);

    g_window = CreateWindow(
            s_windowClass.lpszClassName,
            "Music Game", // title
            WS_OVERLAPPEDWINDOW,
            0, 0, s_width, s_height, // x, y, width, height
            nullptr,
            nullptr,
            g_instance,
            nullptr
    );

    ShowWindow(g_window, SW_SHOWDEFAULT);
    UpdateWindow(g_window);

    rendererInit();
    audioInit();
}

void Platform::free() {
    audioFree();
    rendererFree();

    DestroyWindow(g_window);
    g_window = nullptr;

    UnregisterClass(s_windowClass.lpszClassName, s_windowClass.hInstance);
    ZeroMemory(&s_windowClass, sizeof(s_windowClass));

    CoUninitialize();
}

void Platform::handleEvents() {
    MSG msg {};

    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (s_resizeDirty) {
        s_resizeDirty = false;
        rendererUpdateResolution(s_width, s_height);
    }
}

bool Platform::wantsToQuit() {
    return s_wantsToQuit;
}

char* wcharToChar(wchar_t* source) {
    size_t sourceSize = wcslen(source) + 1;
    size_t newSize = sourceSize * 2;
    size_t convertedChars {};
    char* result = new char[newSize];
    wcstombs_s(&convertedChars, result, newSize, source, _TRUNCATE);
    return result;
}

//Returns the last Win32 error, in string format. Returns an empty string if there is no error.
void printLastError() {
    //Get the error message ID, if any.
    DWORD errorMessageID = ::GetLastError();
    if(errorMessageID == 0) {
        return; //No error message has been recorded
    }

    LPSTR messageBuffer = nullptr;

    //Ask Win32 to give us the string version of that message ID.
    //The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    printf("%s", messageBuffer);

    //Free the Win32's string's buffer.
    LocalFree(messageBuffer);
}

static LRESULT WindowEventHandler(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    if (rendererProcessInput(window, message, wParam, lParam)) {
        return true;
    }

    switch (message) {
        case WM_SIZE: {
            if (wParam == SIZE_MINIMIZED) {
                return 0;
            }

            s_width = static_cast<UINT>(LOWORD(lParam));
            s_height = static_cast<UINT>(HIWORD(lParam));
            s_resizeDirty = true;

            return 0;
        }
        case WM_DESTROY: {
            PostQuitMessage(0);
            s_wantsToQuit = true;
            return 0;
        }
        default : {
            return DefWindowProc(window, message, wParam, lParam);
        }
    }
}

#endif // _WIN32