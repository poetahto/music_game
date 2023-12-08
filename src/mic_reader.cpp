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

struct EkfState {
    float amplitude;
    float frequency;
    float phase;
};

void updateEkf(EkfState state, float input) {
}



void MicReader::init() {

}

void MicReader::free() {

}

void MicReader::showDebugWindow() {
    ImGui::Begin("Mic Reader");

    if (ImGui::Button("Refresh Devices")) {
        Platform::Audio::refreshDeviceList();
    }

    u32 deviceCount = Platform::Audio::getDeviceCount();

    for (int i = 0; i < deviceCount; ++i) {
        Platform::Audio::DeviceInfo* device = Platform::Audio::getDeviceInfo(i);
        ImGui::Text("[%s:%s] %s", device->state, device->dataFlow, device->name);
    }

    ImGui::End();
}
