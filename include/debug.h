#pragma once

#include "moti/moti.h"

namespace moti {
    inline void debugBreak() {
#if MOTI_COMPILER_MSVC
        __debugbreak();
#else
        __builtin_trap();
#endif
    }

    inline void debugOutput(const char* _out) {
#if MOTI_PLATFORM_WINDOWS
        OutputDebugStringA(_out);
#endif
    }
}