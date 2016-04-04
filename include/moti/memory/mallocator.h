#pragma once
#include "allocator.h"
#include <stdlib.h>

namespace moti {
    namespace memory {
        class Mallocator : public Allocator {
        public:
            Mallocator() = default;
            ~Mallocator() = default;
            virtual Block allocate(uint32_t _bytes) override;
            virtual bool reallocate(Block& _block, uint32_t _bytes) override;
            virtual void deallocate(Block& _block) override;
        };
    }
}

