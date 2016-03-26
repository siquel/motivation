#pragma once

namespace moti {
    namespace memory {

        template <size_t MaxSize>
        StackAllocator<MaxSize>::StackAllocator() noexcept
            : m_pointer(m_data) {

        }

        template <size_t MaxSize>
        Block StackAllocator<MaxSize>::allocate(size_t _bytes) noexcept {
            Block result;
            if (_bytes == 0) return result;
            // out of mem
            if (m_pointer + _bytes > m_data + MaxSize) {
                return result;
            }
            result.m_ptr = m_ptr;
            result.m_length = _bytes;
            m_pointer += _bytes;

            return result;
        }


        template <size_t MaxSize>
        Block StackAllocator<MaxSize>::deallocate(Block& _block) noexcept {
            // nullptr
            if (!_block) return;

            
        }




        template <size_t MaxSize>
        bool StackAllocator<MaxSize, Alignment>::owns(const Block& _block) const noexcept {
            return _block && (_block.m_ptr >= m_data && _block.m_ptr < m_data + MaxSize);
        }


    }
}