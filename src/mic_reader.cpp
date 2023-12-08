#include "mic_reader.hpp"
#include "platform.hpp"
#include <imgui.h>

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

static void freeCaptureDevices();

static const Audio::DeviceInfo** s_captureDevices {};
static u32 s_captureDeviceCount {};

static void refreshCaptureDevices() {
    Audio::refreshDeviceList();
    u32 deviceCount = Audio::getDeviceCount();

    for (int i = 0; i < deviceCount; ++i) {
        const Audio::DeviceInfo* device = Audio::getDeviceInfo(i);

        if (device->dataFlow == Audio::DeviceInfo::Capture && device->state == Audio::DeviceInfo::Active) {
            ++s_captureDeviceCount;
        }
    }

    freeCaptureDevices();
    s_captureDevices = new const Audio::DeviceInfo*[s_captureDeviceCount] {};
    u32 currentIndex {};

    for (int i = 0; i < deviceCount; ++i) {
        const Audio::DeviceInfo* device = Audio::getDeviceInfo(i);

        if (device->dataFlow == Audio::DeviceInfo::Capture && device->state == Audio::DeviceInfo::Active) {
            s_captureDevices[currentIndex] = device;
            ++currentIndex;
        }
    }
}

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

    for (int i = 0; i < s_captureDeviceCount; ++i) {
        const Audio::DeviceInfo* device = s_captureDevices[i];
        ImGui::Text("[%s:%s] %s",
                    Audio::DeviceInfo::getName(device->state),
                    Audio::DeviceInfo::getName(device->dataFlow),
                    device->name
        );
    }


    ImGui::End();
}

static void freeCaptureDevices() {
    if (s_captureDevices != nullptr) {
        delete[] s_captureDevices;
        s_captureDevices = nullptr;
    }
}
