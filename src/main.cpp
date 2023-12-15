#include "platform.hpp"
#include "mic_reader.hpp"
#include <imgui.h>

int main() {
    Platform::init();

    while (!Platform::wantsToQuit()) {
        Platform::handleEvents();
        Platform::Renderer::startFrame();

        ImGui::ShowDemoWindow();

        Platform::Renderer::endFrame();
    }

    Platform::free();
    return 0;
}