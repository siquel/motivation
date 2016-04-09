#pragma once

#include "macros.h"
#include "platform.h"
#include <assert.h>
#include <stdint.h>
namespace moti {
    // http://cnicholson.net/2011/01/stupid-c-tricks-a-better-sizeof_array/
    template<typename T, size_t N> char(&COUNTOF_REQUIRES_ARRAY_ARGUMENT(const T(&)[N]))[N];
#define MOTI_COUNTOF(_x) sizeof(moti::COUNTOF_REQUIRES_ARRAY_ARGUMENT(_x) )
}

#define FOURCC(_a, _b, _c, _d) ( ( (uint32_t)(_a) | ( (uint32_t)(_b) << 8) | ( (uint32_t)(_c) << 16) | ( (uint32_t)(_d) << 24) ) )

#define MOTI_VERTEX_SHADER_MAGIC   FOURCC('V', 'S', 'H', 0x0)
#define MOTI_FRAGMENT_SHADER_MAGIC FOURCC('F', 'S', 'H', 0x0)

#if _DEBUG
#define _MOTI_TRACE(fmt, ...) for (;;) { moti::trace(__FILE__, uint16_t(__LINE__), fmt "\n", ##__VA_ARGS__); break;}
#define MOTI_TRACE _MOTI_TRACE


#define MOTI_ASSERT(cond, msg, ...)                                                                                       \
        do {                                                                                                              \
            if (!(cond)) {                                                                                                \
                MOTI_TRACE(msg, ##__VA_ARGS__);                                                                           \
                moti::abort(__FILE__, uint16_t(__LINE__), "\nAssertion failed: %s\n\t  " msg "\n", #cond, ##__VA_ARGS__); \
                assert(0 && #cond);                                                                                       \
            }                                                                                                             \
        } while(0)
#endif

#define MOTI_NOOP(...) for(;;) { break; }

#ifndef MOTI_TRACE
#    define MOTI_TRACE(fmt, ...) MOTI_NOOP()
#endif

#ifndef MOTI_ASSERT
#    define MOTI_ASSERT(cond, fmt, ...) MOTI_NOOP()
#endif

#if MOTI_PLATFORM_WINDOWS
#include <Windows.h>
#endif

#include "debug.h"

namespace moti {
    
    struct UniformType {
        enum Enum {
            Vec4,
            Mat3,
            Mat4,
            Count
        };
    };

    struct PredefinedUniform {
        enum Enum {
            View,
            Proj,
            ViewProj,
            Model,
            ModelViewProj,
            Count
        };

        uint32_t m_loc;
        uint16_t m_count;
        uint8_t m_type;
    };

    struct Attribute {
        enum Enum {
            Position,
            Color,
            Count
        };
    };

    struct AttributeType {
        enum Enum {
            Uint8,
            Float,
            Count
        };
    };

    int32_t vsnprintf(char* _str, size_t _count, const char* _format, va_list _argList);
    void trace(const char* path, uint16_t line, const char* fmt, ...);
    void abort(const char* path, uint16_t line, const char* fmt, ...);
    void fatal(const char* format, ...);

    PredefinedUniform::Enum nameToPredefinedUniform(const char* _name);

    const char* predefinedUniformName(PredefinedUniform::Enum _enum);


}