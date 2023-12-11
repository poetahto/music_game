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
        struct InputDeviceInstanceInfo;
        typedef u32 InputDeviceHandle;
        enum DeviceState {Active, Disabled, NotPresent, Unplugged};

        void refreshDeviceLists();

        // input device management (microphones)
        u32 getInputDeviceCount();
        const DeviceInfo* getInputDeviceInfo(u32 index);
        InputDeviceHandle createInputDeviceInstance(u32 index);
        void freeInputDeviceInstance(InputDeviceHandle handle);
        InputDeviceInstanceInfo* getInputDeviceInstanceInfo(InputDeviceHandle handle);

        struct FormatInfo {
            s32 tag;
            s32 channels;
            s32 bytesPerSec;
            s32 samplesPerSec;
            s32 blockAlign;
            s32 bitsPerSample;
            s32 extraSize;
        };
        struct InputDeviceInstanceInfo {
            u32 bufferSize;
            u32 padding;
            u32 period;
            u32 latency;
            FormatInfo formatInfo;
        };
        struct DeviceInfo {
            const char* deviceName;
            DeviceState state;
            u32 index;
        };
    }
}

#endif