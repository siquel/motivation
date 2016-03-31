#pragma once

#include "string_util.h"

namespace moti {
    struct FixedString {

        FixedString() : m_length(0u), m_data(nullptr) {}
        FixedString(const char* _s) : m_length(moti::strlen32(_s)), m_data(_s) {}
        FixedString(const char* _s, const uint32_t _length) : m_length(_length), m_data(_s) {}

        FixedString& operator=(const char* _str) {
            m_length = moti::strlen32(_str);
            m_data = _str;
            return *this;
        }

        bool operator==(const FixedString& _str) {
            return (m_length == _str.m_length && !strncmp(m_data, _str.m_data, m_length));
        }

        uint32_t size() const {
            return m_length;
        }

        const char* data() const {
            return m_data;
        }

        uint32_t m_length;
        const char* m_data;
    };
}