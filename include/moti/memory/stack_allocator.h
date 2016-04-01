#pragma once
#include "allocator.h"

namespace moti {
    namespace memory {
        // Allocator that provides mem from stack
        // MaxSize max number of bytes to be allocated
		template <uint32_t MaxSize, uint32_t Align = 16u>
        class StackAllocator : public Allocator {
            char m_data[MaxSize];
            char* m_pointer;

            bool isLastUsedBlock(const Block& _b);

        public:
			static const uint32_t max_size = MaxSize;
			static const uint32_t alignment = Align;
            StackAllocator();

			Block allocate(uint32_t _bytes) override;

            void deallocate(Block& _block) override;

			bool reallocate(Block& _block, uint32_t _bytes) override;

            bool owns(const Block& _block) const;

            StackAllocator(const StackAllocator&) = delete;
            StackAllocator& operator=(const StackAllocator&) = delete;
            StackAllocator(StackAllocator&&) = delete;
            StackAllocator& operator=(StackAllocator&&) = delete;
            // disable heap
            void* operator new(size_t) = delete;
            void* operator new[](size_t) = delete;
        };
    }
}

#include "inl/stack_allocator.inl"