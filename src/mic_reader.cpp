#include "mic_reader.hpp"
#include "platform.hpp"
#include <imgui.h>
#include <stdio.h>

/*
 * first mega task: determining pitch in real time
 * https://learn.microsoft.com/en-us/windows/win32/coreaudio/wasapi
 * wasapi is gonna be how we actually get input / mic data
 * its in the form of pulse-code modulation (PCM)
 * pitch detection is a big rabbithole: lots of diff ways to do it
 *
 * https://www.aes.org/e-lib/browse.cfm?elib=20719
 * this seems to be the bleeding-edge in realtime pitch detection
 * 
 * https://en.wikipedia.org/wiki/Extended_Kalman_filter
 * important info in the paper above
 *
 * https://www.katjaas.nl/home/home.html
 * heard good things about this site for dsp stuff
 */

using namespace Platform;

static void refreshCaptureDevices();
static void selectDevice(u32 index);
static void freeCaptureDevices();

static const Audio::DeviceInfo** s_activeInputDevices {};
static u32 s_activeInputDeviceCount {};
static s32 s_selectedDevice {};

void MicReader::init() {
    refreshCaptureDevices();
}

void MicReader::free() {
    freeCaptureDevices();
}

void MicReader::showDebugWindow() {
    ImGui::Begin("Mic Reader");

    if (ImGui::Button("Refresh Devices")) {
        refreshCaptureDevices();
    }

    s32 oldSelectedDevice {s_selectedDevice};
    if (ImGui::ListBox("Input Devices", &s_selectedDevice, [](void* data, int idx)-> const char* {return s_activeInputDevices[idx]->deviceName; }, nullptr, s_activeInputDeviceCount)) {
        if (oldSelectedDevice != s_selectedDevice) {
            selectDevice(s_selectedDevice);
        }
    }

    ImGui::End();
}

static void refreshCaptureDevices() {
    freeCaptureDevices();
    Audio::refreshDeviceLists();
    u32 inputDeviceCount = Audio::getInputDeviceCount();

    for (int i = 0; i < inputDeviceCount; ++i) {
        if (Audio::getInputDeviceInfo(i)->state == Platform::Audio::Active) {
            ++s_activeInputDeviceCount;
        }
    }

    s_activeInputDevices = new const Audio::DeviceInfo*[s_activeInputDeviceCount] {};
    u32 index {};

    for (int i = 0; i < inputDeviceCount; ++i) {
        const Audio::DeviceInfo* device = Audio::getInputDeviceInfo(i);

        if (device->state == Audio::DeviceState::Active) {
            s_activeInputDevices[index] = device;
            ++index;
        }
    }

    selectDevice(0);
}

static void selectDevice(u32 index) {
    printf("Selected %s\n", s_activeInputDevices[index]->deviceName);
}

static void freeCaptureDevices() {
    if (s_activeInputDevices != nullptr) {
        delete[] s_activeInputDevices;
        s_activeInputDevices = nullptr;
    }

    s_selectedDevice = 0;
    s_activeInputDeviceCount = 0;
}
