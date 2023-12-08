#ifdef _WIN32

#include "platform.hpp"
#include "platform_windows.hpp"

#include <d3d11.h>
#include <imgui.h>
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_dx11.h>
#include <stdio.h>

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static void CreateRenderTarget();
static void CleanupRenderTarget();

static ID3D11Device* s_device {};
static ID3D11DeviceContext* s_deviceContext {};
static IDXGISwapChain* s_swapChain {};
static ID3D11RenderTargetView* s_renderTarget {};

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

void rendererInit() {
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
    sd.OutputWindow = g_window;
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
    CreateRenderTarget();

    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(g_window);
    ImGui_ImplDX11_Init(s_device, s_deviceContext);
}

void rendererFree() {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupRenderTarget();
    if (s_swapChain) { s_swapChain->Release(); s_swapChain = nullptr; }
    if (s_deviceContext) { s_deviceContext->Release(); s_deviceContext = nullptr; }
    if (s_device) { s_device->Release(); s_device = nullptr; }
}

bool rendererProcessInput(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    return ImGui_ImplWin32_WndProcHandler(window, message, wParam, lParam);
}

void rendererUpdateResolution(u32 width, u32 height) {
    CleanupRenderTarget();
    s_swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
    CreateRenderTarget();
}

static void CleanupRenderTarget() {
    if (s_renderTarget) {
        s_renderTarget->Release(); s_renderTarget = nullptr;
    }
}

static void CreateRenderTarget() {
    ID3D11Texture2D* pBackBuffer;
    s_swapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    s_device->CreateRenderTargetView(pBackBuffer, nullptr, &s_renderTarget);
    pBackBuffer->Release();
}

#endif // _WIN32