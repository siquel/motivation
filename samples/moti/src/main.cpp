#include "moti/moti.h"
#include "moti/memory/stack_allocator.h"
#include <assert.h>

int main() {
    MOTI_TRACE("sdffsdsdf %d", 44);
    namespace mem = moti::memory;
    constexpr size_t size = 32u;
    mem::StackAllocator<size> alloc;
    mem::Block block = alloc.allocate(size/2);
    assert(alloc.reallocate(block, size));
    assert(block.m_length == size);
    alloc.deallocate(block);
    assert(alloc.reallocate(block, size));
    
    return 0;
}