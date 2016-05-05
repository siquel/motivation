#pragma once

namespace moti {

    template <uint32_t MaxSize, uint32_t Align>
    StackAllocator<MaxSize, Align>::StackAllocator()
        : m_pointer(m_data) {

    }

    template <uint32_t MaxSize, uint32_t Align>
    Block StackAllocator<MaxSize, Align>::allocate(uint32_t _bytes) {
        Block result;
        if (_bytes == 0) return result;

        const uint32_t alignedLength = roundToAlignment(Align, _bytes);
        // out of mem
        if (m_pointer + alignedLength > m_data + MaxSize) {
            return result;
        }
        result.m_ptr = m_pointer;
        result.m_length = alignedLength;
        m_pointer += alignedLength;

        return result;
    }


    template <uint32_t MaxSize, uint32_t Align>
    void StackAllocator<MaxSize, Align>::deallocate(Block& _block) {
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

    template <uint32_t MaxSize, uint32_t Align>
    bool StackAllocator<MaxSize, Align>::reallocate(Block& _block, uint32_t _bytes) {
        if (_block.m_length == _bytes) return true;

        if (_bytes == 0u) return true;

        if (!_block) {
            _block = allocate(_bytes);
            return true;
        }

        const uint32_t alignedLength = roundToAlignment(Align, _bytes);

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

    template <uint32_t MaxSize, uint32_t Align>
    bool StackAllocator<MaxSize, Align>::owns(const Block& _block) const {
        return _block && (_block.m_ptr >= m_data && _block.m_ptr < m_data + MaxSize);
    }


    template <uint32_t MaxSize, uint32_t Align>
    bool StackAllocator<MaxSize, Align>::isLastUsedBlock(const Block& _b) {
        return (static_cast<char *>(_b.m_ptr) + _b.m_length == m_pointer);
    }



}