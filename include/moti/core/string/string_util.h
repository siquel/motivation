#pragma once

#include <algorithm>
#include <stdint.h>

namespace moti {
    inline uint32_t strlen32(const char* _str) {
        return static_cast<uint32_t>(std::strlen(_str));
    }
}

