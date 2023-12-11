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
static void freeCaptureDevices();
static void renderFormatInfo(Audio::FormatInfo* formatInfo);

static const Audio::DeviceInfo** s_activeInputDevices {};
static u32 s_activeInputDeviceCount {};
static s32 s_selectedDevice {};
static Audio::InputDeviceHandle s_currentDevice;

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
            Audio::freeInputDeviceInstance(s_currentDevice);
            s_currentDevice = Audio::createInputDeviceInstance(s_activeInputDevices[s_selectedDevice]->index);
        }
    }

    Audio::InputDeviceInstanceInfo* info = Audio::getInputDeviceInstanceInfo(s_currentDevice);
    ImGui::Text("Buffer Size: %i", info->bufferSize);
    ImGui::Text("Latency: %i", info->latency);
    ImGui::Text("Padding: %i", info->padding);
    ImGui::Text("Period: %i", info->period);
    renderFormatInfo(&info->formatInfo);

    ImGui::End();
}

static void renderFormatInfo(Audio::FormatInfo* formatInfo) {
    ImGui::Text("Bits per sample: %i", formatInfo->bitsPerSample);
    ImGui::Text("Block align: %i", formatInfo->blockAlign);
    ImGui::Text("Bytes per sec: %i", formatInfo->bytesPerSec);
    ImGui::Text("Channels: %i", formatInfo->channels);
    ImGui::Text("Extra size: %i", formatInfo->extraSize);
    ImGui::Text("Samples per sec: %i", formatInfo->samplesPerSec);
    ImGui::Text("Tag: %i", formatInfo->tag);
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

    s_currentDevice = Audio::createInputDeviceInstance(s_activeInputDevices[0]->index);
}

static void freeCaptureDevices() {
    if (s_activeInputDevices != nullptr) {
        delete[] s_activeInputDevices;
        s_activeInputDevices = nullptr;
        Audio::freeInputDeviceInstance(s_currentDevice);
    }
    s_selectedDevice = 0;
    s_activeInputDeviceCount = 0;
}
