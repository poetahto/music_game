#include "platform.hpp"

static const char* s_stateNames[] {"Active", "Disabled", "NotPresent", "Unplugged"};

const char* Platform::Audio::getStateName(Platform::Audio::DeviceState state) {
    return s_stateNames[state];
}
