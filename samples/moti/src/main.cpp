#include "moti/moti.h"
#include "moti/memory/stack_allocator.h"
#include <assert.h>
#include <SDL/SDL.h>

int main(int argc, char** argv) {
    MOTI_TRACE("sdffsdsdf %d", 44);
    namespace mem = moti::memory;
    constexpr size_t size = 32u;
    mem::StackAllocator<size> alloc;
    mem::Block block = alloc.allocate(size/2);
    assert(alloc.reallocate(block, size));
    assert(block.m_length == size);
    alloc.deallocate(block);
    assert(alloc.reallocate(block, size));
    
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* wnd = SDL_CreateWindow("moti", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
    SDL_DestroyWindow(wnd);

    return 0;
}