#pragma once

#include "moti/core/container/array.h"
#include "fixed_string.h"

namespace moti {
    struct DynamicString {
        Array<char> m_data;

        DynamicString(memory::Allocator& _allocator);
        DynamicString(const char* _str, memory::Allocator& _allocator);

        DynamicString& operator=(const DynamicString& _string);
        DynamicString& operator=(const char* _string);
        DynamicString& operator=(const char _string);
        DynamicString& operator=(const FixedString& _string);

        bool startsWith(const char* s) const;
        bool endsWith(const char* s) const;
        bool empty() const;

        void reserve(uint32_t _size);

        const char* c_str() const;
    };
}

