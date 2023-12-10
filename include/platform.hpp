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
        typedef u32 InputDeviceHandle;

        enum DeviceState {Active, Disabled, NotPresent, Unplugged};

        const char* getStateName(DeviceState state);
        void refreshDeviceLists();

        u32 getInputDeviceCount();
        const DeviceInfo* getInputDeviceInfo(u32 index);
        InputDeviceHandle createInputDevice(u32 index);
        void freeInputDevice(InputDeviceHandle device);

        struct DeviceInfo {
            const char* deviceName;
            DeviceState state;
            u32 index;
        };
    }
}

#endif