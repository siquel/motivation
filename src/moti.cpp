#pragma once

#include "moti/moti.h"
#include <stdio.h>
#include <stdarg.h>

namespace moti {

    static const char* s_predefinedUniformNames[] = {
        "u_viewRect",
        "u_view",
        "u_proj",
        "u_viewProj",
        "u_model",
        "u_modelViewProj"
    };

    static_assert(MOTI_COUNTOF(s_predefinedUniformNames) == PredefinedUniform::Count, "Uniform names size differs from enum");

    int32_t vsnprintf(char* _str, size_t _count, const char* _format, va_list _argList) {
        int32_t len = ::vsnprintf_s(_str, _count, size_t(-1), _format, _argList);
        return -1 == len ? ::_vscprintf(_format, _argList) : len;
    }

    void trace(const char* _path, uint16_t _line, const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        char temp[8192];
        char* out = temp;
        int32_t len = moti::vsnprintf(out, sizeof(temp), fmt, args);
        out[len] = '\0';
        va_end(args);
        debugOutput(out);
    }

    void abort(const char* path, uint16_t line, const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);
       
    }

    void fatal(const char* format, ...) {
        moti::debugBreak();
    }

    PredefinedUniform::Enum nameToPredefinedUniform(const char* _name) {
        for (uint32_t i = 0; i < MOTI_COUNTOF(s_predefinedUniformNames); ++i) {
            if (strcmp(_name, s_predefinedUniformNames[i]) == 0) {
                return PredefinedUniform::Enum(i);
            }
        }
        return PredefinedUniform::Count;
    }

    const char* predefinedUniformName(PredefinedUniform::Enum _enum) {
        return s_predefinedUniformNames[_enum];
    }

}
