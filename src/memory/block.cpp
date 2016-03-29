#pragma once

#include "moti/memory/block.h"
#include <utility>

namespace moti {
    namespace memory {

        Block::Block() 
            : m_ptr(nullptr), m_length(0u) {
        }

		Block::Block(void* _ptr, uint32_t _length)
            : m_ptr(_ptr), m_length(_length) {

        }

        Block::Block(Block&& b) {
            *this = std::move(b);
        }

        Block::~Block() {}

        void Block::reset() {
            m_ptr = nullptr;
            m_length = 0u;
        }

        Block::operator bool() const {
            return m_length != 0u;
        }

        bool Block::operator==(const Block & rhs) const {
            return m_ptr == rhs.m_ptr && m_length == rhs.m_length;
        }

        Block & Block::operator=(Block&& b) {
            m_ptr = b.m_ptr;
            m_length = b.m_length;
            b.reset();
            return *this;
        }

    }
}