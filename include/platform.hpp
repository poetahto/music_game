#ifndef MG_PLATFORM_HPP
#define MG_PLATFORM_HPP

#include "types.hpp"

namespace Platform {
    void init();
    void free();
    void handleEvents();
    bool wantsToQuit();

    namespace Renderer {
        void startFrame();
        void endFrame();
    }

    namespace Audio {
        struct DeviceInfo;
        struct InputDeviceHandle {};
        struct OutputDeviceHandle {};
        enum DeviceState {Active, Disabled, NotPresent, Unplugged};

        const char* getStateName(DeviceState state);
        void refreshDeviceLists();

        u32 getInputDeviceCount();
        u32 getOutputDeviceCount();
        const DeviceInfo* getInputDeviceInfo(u32 index);
        const DeviceInfo* getOutputDeviceInfo(u32 index);
        InputDeviceHandle createInputDevice(u32 index);
        OutputDeviceHandle createOutputDevice(u32 index);
        void freeInputDevice(InputDeviceHandle device);
        void freeOutputDevice(OutputDeviceHandle device);

        struct DeviceInfo {
            const char* deviceName;
            DeviceState state;
            u32 index;
        };
    }
}

#endif