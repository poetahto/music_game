#include "platform.hpp"

static const char* s_stateNames[] {"Active", "Disabled", "NotPresent", "Unplugged"};
static const char* s_dataFlowNames[] {"Capture", "Render"};

const char* Platform::Audio::DeviceInfo::getName(Platform::Audio::DeviceInfo::State state) {
    return s_stateNames[state];
}

const char* Platform::Audio::DeviceInfo::getName(Platform::Audio::DeviceInfo::DataFlow dataFlow) {
    return s_dataFlowNames[dataFlow];
}
