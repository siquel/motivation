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
    { -1.0f,  1.0f,  1.0f, 0xff000000 },
    { 1.0f,  1.0f,  1.0f, 0xff0000ff },
    { -1.0f, -1.0f,  1.0f, 0xff00ff00 },
    { 1.0f, -1.0f,  1.0f, 0xff00ffff },
    { -1.0f,  1.0f, -1.0f, 0xffff0000 },
    { 1.0f,  1.0f, -1.0f, 0xffff00ff },
    { -1.0f, -1.0f, -1.0f, 0xffffff00 },
    { 1.0f, -1.0f, -1.0f, 0xffffffff },
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
        uniform vec4 u_viewRect;                    \
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

const int Width = 1280;
const int Height = 720;

int main(int argc, char** argv) {
    using moti::Attribute;
    using moti::AttributeType;
    using moti::Mat4;
    using moti::Vec3;
    using moti::UniformType;

    moti::memory_globals::init();
   
    mem::StackAllocator<4096> alloc;

    SDL_Init(SDL_INIT_VIDEO);
    moti::gl::GLContext context;
    SDL_Window* wnd = SDL_CreateWindow("moti", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Width, Height, SDL_WINDOW_OPENGL);
    context.create(wnd);
    glClearColor(0.0f, 0.f, 0.f, 1.f);

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

    moti::Mat4 view;
    moti::look(view, Vec3{ 0.f, 0.f, -4.f }, Vec3{ 0.f, 0.f, -0.f }, Vec3{ 0.f, 1.f, 0.f });
    moti::Mat4 projection;
    moti::perspective(projection, 60.f, float(Width) / float(Height), 0.1f, 100.f);

    mg::UniformHandle u_time = device.createUniform(UniformType::Vec4, 1, "u_time");

    SDL_Event e;
    bool running = true;

    while (running) {

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float time = SDL_GetTicks() / 1000.f;
        float angle = time * 45.f;  // 45° per second
        
        for (uint32_t y = 0; y < 11; ++y) {
            for (uint32_t x = 0; x < 11; ++x) {
                moti::Mat4 model;
                model.setIdentity();
                translate(model, Vec3{ -15.f + float(x) * 3.f, -15.f + 3.f * float(y), 0.f });
                moti::rotate(model, moti::radians(angle), Vec3{ 1.f, 1.f, 0.f });

                device.setIndexBuffer(ibo, 0, 36);
                device.setVertexBuffer(vbo, 0, 8);
                device.setViewTransform(view, projection);
                device.setViewRect(0, 0, Width, Height);
                device.setTransform(model);
                device.setUniform(u_time, &time, 1);
                device.submit(p);
            }
        }
        
        
        SDL_GL_SwapWindow(wnd);
    }

    device.destroyUniform(u_time);
    device.destroyShader(vsh);
    device.destroyShader(fsh);
    device.destroyVertexBuffer(vbo);
    device.destroyIndexBuffer(ibo);
    device.destroyProgram(p);
    SDL_DestroyWindow(wnd);
    SDL_Quit();

    moti::memory_globals::shutdown();

    return 0;
}