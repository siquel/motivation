#pragma once
#include "moti/memory/block.h"

namespace moti {
    namespace memory {
        // Allocator that provides mem from stack
        // MaxSize max number of bytes to be allocated
        template <size_t MaxSize, size_t Align = 16u>
        class StackAllocator {
            char m_data[MaxSize];
            char* m_pointer;

            bool isLastUsedBlock(const Block& _b);

        public:
            static const size_t max_size = MaxSize;
            static const size_t alignment = Align;
            StackAllocator();

            Block allocate(size_t _bytes);

            void deallocate(Block& _block);

            bool reallocate(Block& _block, size_t _bytes) ;

            bool owns(const Block& _block) const;

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