#include "moti/moti.h"
#include "moti/memory/memory.h"
#include "moti/memory/mallocator.h"
#include "moti/memory/linear_allocator.h"
#include <new>
namespace moti {
    namespace memory_globals {

        using DefaultMemoryAllocator = Mallocator;
        using DefaultScratchAllocator = LinearAllocator;

        struct MemoryGlobals {
            static const int AllocatorMemory = sizeof(DefaultMemoryAllocator) + sizeof(DefaultScratchAllocator);
            char m_buffer[AllocatorMemory];
            DefaultMemoryAllocator* m_defaultAllocator;
            DefaultScratchAllocator* m_defaultScratchAllocator;
        };

        static MemoryGlobals s_memoryGlobals;

        void init(uint32_t _bufferSize /* = 4 * 1024 * 1024 */) {
            char* mem = s_memoryGlobals.m_buffer;
            s_memoryGlobals.m_defaultAllocator = new (mem) DefaultMemoryAllocator;
            mem += sizeof(DefaultMemoryAllocator);
            s_memoryGlobals.m_defaultScratchAllocator = new (mem) DefaultScratchAllocator(*s_memoryGlobals.m_defaultAllocator,_bufferSize);
        }

        void shutdown() {
            s_memoryGlobals.m_defaultScratchAllocator->~DefaultScratchAllocator();
            s_memoryGlobals.m_defaultAllocator->~DefaultMemoryAllocator();
            memset(&s_memoryGlobals, 0, sizeof(MemoryGlobals));
        }

        Allocator& defaultAllocator() {
            return *s_memoryGlobals.m_defaultAllocator;
        }

        Allocator& defaultScratchAllocator() {
            return *s_memoryGlobals.m_defaultScratchAllocator;
        }
    }
}