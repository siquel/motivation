#pragma once
#include "moti/memory/block.h"

namespace moti {
    namespace memory {
        // Allocator that provides mem from stack
        // MaxSize max number of bytes to be allocated
        template <size_t MaxSize>
        class StackAllocator {
            char m_data[MaxSize];
            char* m_pointer;
        public:
            static const size_t max_size = MaxSize;
            static const size_t alignment = Alignment;
            StackAllocator() noexcept;

            Block allocate(size_t _bytes) noexcept;

            Block deallocate(Block& _block) noexcept;

            bool reallocate(Block& _block, size_t _bytes) noexcept;

            bool owns(const Block& _block) const noexcept;

            StackAllocator(const StackAllocator&) = delete;
            StackAllocator& operator=(const StackAllocator&) = delete;
            StackAllocator(StackAllocator&&) = delete;
            StackAllocator& operator=(StackAllocator&&) = delete;
            // disable heap
            void* operator new(size_t) = delete;
            void* operator new[](size_t) = delete;
            void operator delete(void*) = delete;
            void operator delete[](void*) = delete;
            // no addr
            StackAllocator* operator&() = delete;
        };
    }
}

#include "inl/stack_allocator.inl"