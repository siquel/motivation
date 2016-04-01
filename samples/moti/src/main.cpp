#include "moti/moti.h"
#include "moti/memory/stack_allocator.h"
#include <assert.h>
#include <SDL/SDL.h>
#include "moti/renderer/gl_context.h"
#include "app.h"
#include "moti/renderer/graphics_device.h"
#include "moti/core/string/dynamic_string.h"
#include "moti/memory/scratch_allocator.h"
namespace mem = moti::memory;
namespace mg = moti::graphics;


struct PosColorVertex {
    float m_x;
    float m_y;
    float m_z;
    uint32_t rgba;
};

static PosColorVertex s_vertices[3] = {
    {-1.0, -1.0f, 0.0f, 0x00FF00FF},
    { 1.0, -1.0f, 0.0f, 0x00FF00FF },
    { 1.0, 1.0f, 0.0f, 0x00FF00FF }
};

mg::VertexDecl s_decl;

struct POD {
    uint32_t top;
    uint32_t kek;
};

int main(int argc, char** argv) {
    const size_t size = sizeof(s_vertices);
    const size_t gg = sizeof(POD) * 2;
    mem::StackAllocator<gg> test_alloc;
    mem::ScratchAllocator allocator(test_alloc, gg);
    mem::Block block = allocator.allocate(sizeof(POD));
    mem::Block block2 = allocator.allocate(sizeof(POD));
    POD* pod1 = new (block.m_ptr)POD;
    POD* pod2 = new (block2.m_ptr)POD;
    pod1->top = 1337; pod1->kek = 80085;
    pod2->top = 7331; pod2->kek = 58008;
    
    allocator.deallocate(block2);

    mem::StackAllocator<size> alloc;
    mem::Block memory = alloc.allocate(size);
    std::memcpy(memory.m_ptr, s_vertices, size);

    s_decl.begin()
        .add(mg::Attribute::Position, 3, mg::AttributeType::Float, true)
        .add(mg::Attribute::Color, 4, mg::AttributeType::Uint8, false);

    SDL_Init(SDL_INIT_VIDEO);
    moti::gl::GLContext context;
    SDL_Window* wnd = SDL_CreateWindow("moti", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
    context.create(wnd);
    glClearColor(1.f, 0.f, 0.f, 1.f);

    mg::GraphicsDevice device;
    
    mg::VertexBufferHandle vbo = device.createVertexBuffer(&memory, s_decl);

    SDL_Event e;
    while (SDL_WaitEvent(&e)) {
        if (e.type == SDL_QUIT) break;
        
        device.setVertexBuffer(vbo);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        SDL_GL_SwapWindow(wnd);
    }

    SDL_DestroyWindow(wnd);
    SDL_Quit();

    return 0;
}