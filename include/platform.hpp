#ifndef MG_PLATFORM_HPP
#define MG_PLATFORM_HPP

#include "types.hpp"

namespace Platform {
    void init();
    void free();
    void handleEvents();
    bool wantsToQuit();
    void sleep(u32 duration);

    namespace Renderer {
        void startFrame();
        void endFrame();
    }

    namespace Audio {
        struct DeviceInfo;

        void refreshDeviceList();
        u32 getDeviceCount();
        DeviceInfo* getDeviceInfo(u32 index);

        struct DeviceInfo {
            const char* name;
            const char* state;
            const char* dataFlow;
        };
    }
}

#endif