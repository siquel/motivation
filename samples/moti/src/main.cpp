#include "moti/moti.h"
#include "moti/memory/stack_allocator.h"
#include <assert.h>
#include <SDL/SDL.h>
#include "moti/renderer/gl_context.h"
#include "app.h"
#include "moti/renderer/graphics_device.h"
#include "moti/core/container/array.h"

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

void f(const moti::Array<POD>& a) {
    auto b = a.front();
    auto c = a.end();
    auto d = a.begin();
    auto h = a.back();
}

int main(int argc, char** argv) {
    const size_t size = sizeof(s_vertices);
    
    mem::StackAllocator<size> array_alloc;

    moti::Array<POD> arr(array_alloc, 2);
    arr.push_back({ 1337, 715517 });
    arr.push_back({ 80085, 5555 });
    arr.push_back({ 80085, 2225555 });
    f(arr);
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