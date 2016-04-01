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

        uint32_t size() const;

        const char* c_str() const;
    };
}

namespace moti {

    inline DynamicString::DynamicString(memory::Allocator& _allocator)
        : m_data(_allocator) {

    }

    inline DynamicString::DynamicString(const char* _str, memory::Allocator& _allocator) 
        : m_data(_allocator) {
        MOTI_ASSERT(_str != nullptr, "String cannot be null");
        m_data.push(_str, strlen32(_str));
    }

    inline DynamicString& DynamicString::operator=(const FixedString& _string) {
        m_data.clear();
        m_data.push(_string.data(), _string.size());
        return *this;
    }

    inline DynamicString& DynamicString::operator=(const char _string) {
        m_data.clear();
        m_data.push_back(_string);
        return *this;
    }

    inline DynamicString& DynamicString::operator=(const char* _string) {
        m_data.clear();
        m_data.push(_string, moti::strlen32(_string));
        return *this;
    }

    inline DynamicString& DynamicString::operator=(const DynamicString& _string) {
        m_data = _string.m_data;
        return *this;
    }

    inline bool DynamicString::startsWith(const char* s) const {

    }

    inline bool DynamicString::endsWith(const char* s) const {

    }

    inline bool DynamicString::empty() const {
        return m_data.m_size == 0;
    }

    inline void DynamicString::reserve(uint32_t _size) {
        m_data.reserve(_size);
    }

    uint32_t DynamicString::size() const {
        return m_data.m_size;
    }

    inline const char* DynamicString::c_str() const {
        auto data = const_cast<Array<char>&>(m_data);
        data.push_back('\0');
        data.pop_back();
        return data.begin();
    }

    inline bool operator==(const DynamicString& _a, const DynamicString& _b) {
        return strcmp(_a.c_str(), _b.c_str()) == 0;
    }
}