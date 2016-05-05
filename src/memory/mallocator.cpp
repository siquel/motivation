#include "moti/memory/mallocator.h"

namespace moti {
    Block Mallocator::allocate(uint32_t _bytes) {
        Block result;
        if (_bytes == 0) return result;

        void* p = ::malloc(_bytes);
        if (p != nullptr) {
            result.m_ptr = p;
            result.m_length = _bytes;
        }
        return result;
    }

    bool Mallocator::reallocate(Block& _block, uint32_t _bytes) {

        if (_bytes == _block.m_length) return true;
        if (_bytes == 0) {
            deallocate(_block);
            return true;
        }

        if (!_block) {
            _block = allocate(_bytes);
            return true;
        }

        Block reallocated(::realloc(_block.m_ptr, _bytes), _bytes);
        if (reallocated) {
            _block = reallocated;
            return true;
        }
        return false;
    }

    void Mallocator::deallocate(Block& _block) {
        if (_block) {
            ::free(_block.m_ptr);
            _block.reset();
        }
    }

}

