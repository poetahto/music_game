#ifdef _WIN32

#include "platform.hpp"

#define WIN32_MEAN_AND_LEAN
#include <windows.h>
#include <d3d11.h>
#include <stdio.h>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_win32.h>
#include <imgui/backends/imgui_impl_dx11.h>

static int s_width {800};
static int s_height {600};
static bool s_wantsToQuit {false};

static HINSTANCE s_instance {};
static HWND s_window {};
static WNDCLASS s_windowClass {};

static ID3D11Device* s_device {};
static ID3D11DeviceContext* s_deviceContext {};
static IDXGISwapChain* s_swapChain {};
static ID3D11RenderTargetView* s_renderTarget {};

static LRESULT WindowEventHandler(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

void Platform::init() {
    s_instance = GetModuleHandle(nullptr);

    // Create the win32 window
    {
        s_windowClass.lpszClassName = "MusicGame";
        s_windowClass.hInstance = s_instance;
        s_windowClass.lpfnWndProc = WindowEventHandler;
        // windowClass.hIcon; // todo: at some point, add this
        RegisterClass(&s_windowClass);

        s_window = CreateWindow(
                s_windowClass.lpszClassName,
                "Music Game", // title
                WS_OVERLAPPEDWINDOW,
                0, 0, s_width, s_height, // x, y, width, height
                nullptr,
                nullptr,
                s_instance,
                nullptr
        );
    }

    // Initialize d3d11
    {
        DXGI_SWAP_CHAIN_DESC sd;
        ZeroMemory(&sd, sizeof(sd));
        sd.BufferCount = 2;
        sd.BufferDesc.Width = 0;
        sd.BufferDesc.Height = 0;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = s_window;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        UINT createDeviceFlags = 0;
        D3D_FEATURE_LEVEL featureLevel;
        const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
        HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &s_swapChain, &s_device, &featureLevel, &s_deviceContext);

        // Try high-performance WARP software driver if hardware is not available.
        if (res == DXGI_ERROR_UNSUPPORTED) {
            res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &s_swapChain, &s_device, &featureLevel, &s_deviceContext);
        }

        if (res != S_OK) {
            printf("Failed to initialize d3d11\n");
        }

        // Create the render target
        ID3D11Texture2D* pBackBuffer;
        s_swapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
        s_device->CreateRenderTargetView(pBackBuffer, nullptr, &s_renderTarget);
        pBackBuffer->Release();
    }

    ShowWindow(s_window, SW_SHOWDEFAULT);
    UpdateWindow(s_window);

    // Setup Dear ImGui
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void) io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        ImGui::StyleColorsDark();
        ImGui_ImplWin32_Init(s_window);
        ImGui_ImplDX11_Init(s_device, s_deviceContext);
    }
}

void Platform::Renderer::startFrame() {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void Platform::Renderer::endFrame() {
    ImGui::Render();

    const float clearColor[] {0, 0, 0, 1};
    s_deviceContext->OMSetRenderTargets(1, &s_renderTarget, nullptr);
    s_deviceContext->ClearRenderTargetView(s_renderTarget, clearColor);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    s_swapChain->Present(1, 0);
}

void Platform::free() {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    if (s_renderTarget) { s_renderTarget->Release(); s_renderTarget = nullptr; }
    if (s_swapChain) { s_swapChain->Release(); s_swapChain = nullptr; }
    if (s_deviceContext) { s_deviceContext->Release(); s_deviceContext = nullptr; }
    if (s_device) { s_device->Release(); s_device = nullptr; }

    DestroyWindow(s_window);
    s_window = nullptr;
    UnregisterClass(s_windowClass.lpszClassName, s_windowClass.hInstance);
    ZeroMemory(&s_windowClass, sizeof(s_windowClass));
}

void Platform::sleep(u32 duration) {
    Sleep(duration);
}

bool Platform::wantsToQuit() {
    return s_wantsToQuit;
}

void Platform::handleEvents() {
    MSG msg {};
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static LRESULT WindowEventHandler(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(window, message, wParam, lParam)) {
        return true;
    }

    switch (message) {
        case WM_SIZE: {
            if (wParam == SIZE_MINIMIZED) {
                return 0;
            }
            s_width = static_cast<UINT>(LOWORD(lParam));
            s_height = static_cast<UINT>(HIWORD(lParam));

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