#pragma once
#include "allocator.h"

namespace moti {
    class LinearAllocator : public Allocator {
    private:
        Block m_buffer;
        Allocator* m_backing;
        uint32_t m_offset;
        uint32_t m_size;
    public:
        LinearAllocator(Allocator& backing, uint32_t _size);
        ~LinearAllocator();

        Block allocate(uint32_t _bytes) override;

        void deallocate(Block& _block) override;

        bool reallocate(Block& _block, uint32_t _bytes) override;

        void deallocateAll();
    };

}