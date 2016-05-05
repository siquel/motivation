#pragma once


MOTI_FORWARD_DECLARE_STRUCT(moti, Allocator);

namespace moti {

    namespace memory_globals {
        void init(uint32_t _bufferSize = 4 * 1024 * 1024);
        void shutdown();
        Allocator& defaultAllocator();

        Allocator& defaultScratchAllocator();
    }
}