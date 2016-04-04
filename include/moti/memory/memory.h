#pragma once


MOTI_FORWARD_DECLARE_STRUCT(moti, memory, Allocator);

namespace moti {

    namespace memory_globals {
        void init(uint32_t _bufferSize = 4 * 1024 * 1024);
        void shutdown();
        memory::Allocator& defaultAllocator();

        memory::Allocator& defaultScratchAllocator();
    }
}