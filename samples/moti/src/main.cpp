#include "moti/moti.h"
#include "moti/memory/stack_allocator.h"
#include <assert.h>
#include <SDL/SDL.h>
#include "moti/renderer/gl_context.h"
#include "moti/renderer/graphics_device.h"
#include "moti/core/string/dynamic_string.h"
#include "moti/memory/mallocator.h"
#include "moti/memory/linear_allocator.h"
#include "moti/memory/memory.h"
#include "moti/io/io.h"
#include "moti/math/math.h"
namespace mem = moti::memory;
namespace mg = moti::graphics;

struct PosColorVertex {
    float m_x;
    float m_y;
    float m_z;
    uint32_t abgr;
};

static PosColorVertex s_vertices[8] =
{
    { -0.5f,  0.5f,  0.5f, 0xff000000 },
    { 0.5f,  0.5f,  0.5f, 0xff0000ff },
    { -0.5f, -0.5f,  0.5f, 0xff00ff00 },
    { 0.5f, -0.5f,  0.5f, 0xff00ffff },
    { -0.5f,  0.5f, -0.5f, 0xffff0000 },
    { 0.5f,  0.5f, -0.5f, 0xffff00ff },
    { -0.5f, -0.5f, -0.5f, 0xffffff00 },
    { 0.5f, -0.5f, -0.5f, 0xffffffff },
};

static const uint16_t s_indices[36] =
{
    0, 1, 2, // 0
    1, 3, 2,
    4, 6, 5, // 2
    5, 6, 7,
    0, 2, 4, // 4
    4, 2, 6,
    1, 5, 3, // 6
    5, 7, 3,
    0, 4, 1, // 8
    4, 5, 1,
    2, 3, 6, // 10
    6, 3, 7,
};


mg::VertexDecl s_decl;

#define VERT_HEAD  "#version 440\n" MOTI_TO_STRING( \
        uniform mat4 u_view;                        \
        uniform mat4 u_proj;                        \
        uniform mat4 u_viewProj;                    \
        uniform mat4 u_model;                       \
        uniform mat4 u_modelViewProj;               \
)


#define FRAG_HEAD \
"#version 440\n"

static const char* s_VertexShader = VERT_HEAD MOTI_TO_STRING(
    layout(location = 0) in vec3 a_position;
    layout(location = 1) in vec4 a_color;

    out vec4 color;
    void main() {
        color = a_color;
        gl_Position = u_modelViewProj * vec4(a_position, 1.0);
    }
);

static const char* s_FragmentShader = VERT_HEAD MOTI_TO_STRING(
    in vec4 color;
    out vec4 outColor;
    void main() {
        outColor = color;
    }
);

mg::ShaderHandle createShader(const char* src, uint32_t magic, mg::GraphicsDevice& device) {
    moti::memory::StackAllocator<4096> shaderAlloc;
    mem::Block block;
    moti::MemoryWriter writer(&block, &shaderAlloc);
    moti::write<uint32_t>(&writer, magic);
    int32_t len = static_cast<int32_t>(strlen(s_VertexShader));
    moti::write(&writer, len);
    moti::write(&writer, (void*)src, len);
    return device.createShader(&block);
}

int main(int argc, char** argv) {
    using moti::Attribute;
    using moti::AttributeType;


    moti::memory_globals::init();
   
    mem::StackAllocator<4096> alloc;

    SDL_Init(SDL_INIT_VIDEO);
    moti::gl::GLContext context;
    SDL_Window* wnd = SDL_CreateWindow("moti", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
    context.create(wnd);
    glClearColor(0.f, 0.f, 0.f, 1.f);

    mg::GraphicsDevice device;

    mg::ShaderHandle vsh = createShader(s_VertexShader, MOTI_VERTEX_SHADER_MAGIC, device);
    mg::ShaderHandle fsh = createShader(s_FragmentShader, MOTI_FRAGMENT_SHADER_MAGIC, device);
    mg::ProgramHandle p = device.createProgram(vsh, fsh);

    mem::Block memory = alloc.allocate(sizeof(s_vertices));
    memcpy(memory.m_ptr, s_vertices, sizeof(s_vertices));

    s_decl.begin()
        .add(Attribute::Position, 3, AttributeType::Float, true)
        .add(Attribute::Color, 4, AttributeType::Uint8, false);

    mem::Block indicesBlock = alloc.allocate(sizeof(s_indices));
    memcpy(indicesBlock.m_ptr, s_indices, sizeof(s_indices));

    mg::VertexBufferHandle vbo = device.createVertexBuffer(&memory, s_decl);
    mg::IndexBufferHandle ibo = device.createIndexBuffer(&indicesBlock);

    SDL_Event e;
    while (SDL_WaitEvent(&e)) {
        if (e.type == SDL_QUIT) break;
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        device.setIndexBuffer(ibo, 0, 36);
        device.setVertexBuffer(vbo, 0, 8);
        device.submit(p);
        SDL_GL_SwapWindow(wnd);
    }

    device.destroyShader(vsh);
    device.destroyShader(fsh);
    device.destroyProgram(p);
    SDL_DestroyWindow(wnd);
    SDL_Quit();

    moti::memory_globals::shutdown();

    return 0;
}