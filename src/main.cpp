#include <stdio.h>
#include "platform.hpp"

int main() {
    Platform::init();

    while (!Platform::wantsToQuit()) {
        Platform::handleEvents();
        Platform::sleep(100);
    }

    Platform::free();
    return 0;
}