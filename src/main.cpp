#include "platform.hpp"
#include "mic_reader.hpp"
#include <imgui.h>

int main() {
    Platform::init();
    MicReader::init();

    while (!Platform::wantsToQuit()) {
        Platform::handleEvents();
        Platform::Renderer::startFrame();

        MicReader::showDebugWindow();
        ImGui::ShowDemoWindow();

        Platform::Renderer::endFrame();
    }

    MicReader::free();
    Platform::free();
    return 0;
}