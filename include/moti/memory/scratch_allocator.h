#pragma once

#include "allocator.h"

namespace moti {
    namespace memory {
 
        class ScratchAllocator : public Allocator {
        private:
            Block m_buffer;
            char* m_begin, *m_end;

            char* m_allocate, *m_free;

            bool inUse(void* _ptr);
        public:
            ScratchAllocator(Allocator& _allocator, uint32_t _size);
            ~ScratchAllocator() = default;

            Block allocate(uint32_t _bytes) override;

            void deallocate(Block& _block) override;

            bool reallocate(Block& _block, uint32_t _bytes) override;
        };
    }
}

namespace moti {
    namespace memory {



        ScratchAllocator::ScratchAllocator(Allocator& _allocator, uint32_t _size) {
            m_buffer = _allocator.allocate(_size);
            m_begin = static_cast<char*>(m_buffer.m_ptr);
            m_end = m_begin + _size;
            m_free = m_allocate = m_begin;
        }

        bool ScratchAllocator::inUse(void* _ptr) {

            if (m_free == m_allocate) return false;
            if (m_allocate > m_free) return _ptr >= m_free && _ptr < m_allocate;
            return _ptr >= m_free || _ptr < m_allocate;
        }

        Block ScratchAllocator::allocate(uint32_t _bytes) {

            Block result;

            if (_bytes == 0) return result;

            char* p = m_allocate + _bytes;
            
            // reached the end, wrap around
            if (p > m_end) {
                p = m_begin + _bytes;
            }


            if (inUse(p)) {
                MOTI_ASSERT(0, "Backing allocator needed!");
            }

            result.m_ptr = p - _bytes;
            result.m_length = _bytes;
            m_allocate = p;
            return result;
        }

        void ScratchAllocator::deallocate(Block& _block) {
            if (!_block) return;

            void* p = _block.m_ptr;
            if (p < m_begin || p >= m_end) {
                // TODO its allocated by backing allocator
                return;
            }
            
            
        }

        bool ScratchAllocator::reallocate(Block& _block, uint32_t _bytes) {
            return false;
        }
    }
}
