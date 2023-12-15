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
        struct InputBuffer;
        typedef u32 InputDeviceHandle;
        enum DeviceState {Active, Disabled, NotPresent, Unplugged};

        void refreshDeviceLists();

        // input device management (microphones)
        u32 getInputDeviceCount();
        const DeviceInfo* getInputDeviceInfo(u32 index);
        InputDeviceHandle createInputDeviceInstance(u32 index);
        void freeInputDeviceInstance(InputDeviceHandle handle);
        InputDeviceInstanceInfo* getInputDeviceInstanceInfo(InputDeviceHandle handle);
        InputBuffer* captureBuffer(InputDeviceHandle handle);
        void freeBuffer(InputBuffer* buffer);

        struct InputBuffer {
            u8* data {};
            u32 length {};
        };
        struct FormatInfo {
            s32 tag;
            s32 channels;
            s32 bytesPerSec;
            s32 samplesPerSec;
            s32 blockAlign;
            s32 bitsPerSample;
            s32 extraSize;
            s32 samples;
            s32 channelMask;
        };
        struct InputDeviceInstanceInfo {
            u32 bufferSize;
            u32 padding;
            u32 period;
            u32 latency;
            FormatInfo format;
        };
        struct DeviceInfo {
            const char* deviceName;
            DeviceState state;
            u32 index;
        };
    }
}

#endif