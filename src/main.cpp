#include "platform.hpp"
#include <imgui/imgui.h>

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