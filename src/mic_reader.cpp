#include "mic_reader.hpp"
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
 * https://www.katjaas.nl/home/home.html
 * heard good things about this site for dsp stuff
 */

void MicReader::init() {

}

void MicReader::free() {

}

void MicReader::showDebugWindow() {
    ImGui::Begin("Mic Reader");

    ImGui::Text("Work-in-progress!");

    ImGui::End();
}
