#include "moti/moti.h"
#include "moti/memory/stack_allocator.h"
#include <assert.h>
#include <SDL/SDL.h>
#include "moti/renderer/gl_context.h"
#include "app.h"
#include "moti/renderer/graphics_device.h"

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

mg::VertexDecl s_decl{
    sizeof(PosColorVertex)
};

int main(int argc, char** argv) {
    MOTI_TRACE("sdffsdsdf %d", 44);
    const size_t size = sizeof(s_vertices);
    
    mem::StackAllocator<size> alloc;
    mem::Block memory = alloc.allocate(size);
    std::memcpy(memory.m_ptr, s_vertices, size);

    App app;
    mg::GraphicsDevice device;
    
    device.createVertexBuffer(&memory, s_decl);
    return app.exec();
}