#pragma once

#include "moti/core/container/array.h"
#include "fixed_string.h"
#include "moti/moti.h"

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

namespace moti {

    DynamicString::DynamicString(memory::Allocator& _allocator)
        : m_data(_allocator) {

    }

    DynamicString::DynamicString(const char* _str, memory::Allocator& _allocator) 
        : m_data(_allocator) {
        MOTI_ASSERT(_str != nullptr, "String cannot be null");
        m_data.push(_str, strlen32(_str));
    }

    const char* DynamicString::c_str() const {
        auto data = const_cast<Array<char>&>(m_data);
        data.push_back('\0');
        data.pop_back();
        return data.begin();
    }
}