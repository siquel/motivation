#pragma once

namespace moti {
    namespace memory {

        template <size_t MaxSize, size_t Align>
        StackAllocator<MaxSize, Align>::StackAllocator() noexcept
            : m_pointer(m_data) {

        }

        template <size_t MaxSize, size_t Align>
        Block StackAllocator<MaxSize, Align>::allocate(size_t _bytes) noexcept {
            Block result;
            if (_bytes == 0) return result;

            const size_t alignedLength = roundToAlignment(Align, _bytes);
            // out of mem
            if (m_pointer + alignedLength > m_data + MaxSize) {
                return result;
            }
            result.m_ptr = m_pointer;
            result.m_length = alignedLength;
            m_pointer += alignedLength;

            return result;
        }


        template <size_t MaxSize, size_t Align>
        void StackAllocator<MaxSize, Align>::deallocate(Block& _block) noexcept {
            // nullptr
            if (!_block) return;

            if (!owns(_block)) {
                return;
            }

            // if its recent mem block allocated, we can then re-use the existing memroy
            if (isLastUsedBlock(_block)) {
                m_pointer = static_cast<char*>(_block.m_ptr);
            }

            _block.reset();
        }

        template <size_t MaxSize, size_t Align>
        bool StackAllocator<MaxSize, Align>::reallocate(Block& _block, size_t _bytes) noexcept {
            if (_block.m_length == _bytes) return true;

            if (_bytes == 0u) return true;

            if (!_block) {
                _block = allocate(_bytes);
                return true;
            }

            const size_t alignedLength = roundToAlignment(Align, _bytes);

            if (_block.m_length > _bytes) {
                _block.m_length = alignedLength;
                return true;
            }
            
            if (isLastUsedBlock(_block)) {
                // this is the block allocated just now, we can expand it if we have enough memory
                if (static_cast<char*>(_block.m_ptr) + alignedLength <= m_data + MaxSize) {
                    _block.m_length = alignedLength;
                    // assign pointer correctly
                    m_pointer = static_cast<char*>(_block.m_ptr) + alignedLength;
                    return true;
                }
            }

            return false;
        }

        template <size_t MaxSize, size_t Align>
        bool StackAllocator<MaxSize, Align>::owns(const Block& _block) const noexcept {
            return _block && (_block.m_ptr >= m_data && _block.m_ptr < m_data + MaxSize);
        }


        template <size_t MaxSize, size_t Align>
        bool StackAllocator<MaxSize, Align>::isLastUsedBlock(const Block& _b) noexcept {
            return (static_cast<char *>(_b.m_ptr) + _b.m_length == m_pointer);
        }



    }
}