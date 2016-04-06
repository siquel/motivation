#include "moti/moti.h"
#include "moti/memory/linear_allocator.h"

namespace moti {
    namespace memory {
        LinearAllocator::LinearAllocator(Allocator& backing, uint32_t _size)
            : m_backing(&backing), m_offset(0u), m_size(_size) {
            m_buffer = backing.allocate(m_size);
            MOTI_ASSERT(m_buffer, "Couldn't reserve memory for linear allocator");
        }

        LinearAllocator::~LinearAllocator() {
            m_backing->deallocate(m_buffer);
        }

        Block LinearAllocator::allocate(uint32_t _bytes) {

            Block result;

            static const uint32_t Align = 4;
            uint32_t alignedLength = roundToAlignment(Align, _bytes);
            // out of memory
            if (m_offset + alignedLength > m_size) return result;

            result.m_ptr = static_cast<char*>(m_buffer.m_ptr) + m_offset;
            result.m_length = alignedLength;
            m_offset += alignedLength;
            return result;
        }

        void LinearAllocator::deallocate(Block& _block) {

        }

        bool LinearAllocator::reallocate(Block& _block, uint32_t _bytes) {
            return false;
        }


        void LinearAllocator::deallocateAll() {
            m_offset = 0;
        }

    }
}