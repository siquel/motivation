#pragma once

#include "macros.h"
#include "platform.h"
#include <stdint.h>
namespace moti {
    // http://cnicholson.net/2011/01/stupid-c-tricks-a-better-sizeof_array/
    template<typename T, size_t N> char(&COUNTOF_REQUIRES_ARRAY_ARGUMENT(const T(&)[N]))[N];
#define MOTI_COUNTOF(_x) sizeof(moti::COUNTOF_REQUIRES_ARRAY_ARGUMENT(_x) )

    int32_t vsnprintf(char* _str, size_t _count, const char* _format, va_list _argList);
    void trace(const char* path, uint16_t line, const char* fmt, ...);
    void fatal(const char* format, ...);

#if _DEBUG
#define _MOTI_TRACE(fmt, ...) for (;;) { moti::trace(__FILE__, uint16_t(__LINE__), fmt "\n", ##__VA_ARGS__); break;}
#define MOTI_TRACE _MOTI_TRACE
#define MOTI_ASSERT(cond, fmt, ...)                                   \
        for(;;) {                                                     \
            if (!(cond)) {                                              \
                MOTI_TRACE(fmt, ##__VA_ARGS__);                       \
                moti::fatal(fmt, ##__VA_ARGS__);                      \
            }                                                         \
        break;}                                                       

}
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