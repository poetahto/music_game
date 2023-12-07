#ifndef MG_PLATFORM_HPP
#define MG_PLATFORM_HPP

#include "Types.hpp"

namespace Platform {
    void init();
    void free();
    void handleEvents();
    bool wantsToQuit();
    void sleep(u32 duration);
}

#endif