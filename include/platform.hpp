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

        void refreshDeviceList();
        u32 getDeviceCount();
        const DeviceInfo* getDeviceInfo(u32 index);

        struct DeviceInfo {
            enum DataFlow {
                Capture, Render
            } dataFlow;

            enum State {
                Active, Disabled, NotPresent, Unplugged
            } state;

            const char* name;
            u32 index;

            static const char* getName(DataFlow dataFlow);
            static const char* getName(State state);
        };
    }
}

#endif