#pragma once

#include "moti/memory/block.h"

namespace moti {
    struct Allocator {
        virtual ~Allocator() = default;
        virtual Block allocate(uint32_t _bytes) = 0;
        virtual void deallocate(Block& _block) = 0;
        virtual bool reallocate(Block& _block, uint32_t _bytes) = 0;

        Allocator(const Allocator&) = delete;
        Allocator& operator=(const Allocator&) = delete;
        Allocator(Allocator&&) = delete;
        Allocator& operator=(const Allocator&&) = delete;
    protected:
        Allocator() = default;
    };
}