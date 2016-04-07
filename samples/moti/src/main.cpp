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

namespace mem = moti::memory;
namespace mg = moti::graphics;

struct PosColorVertex {
    float m_x;
    float m_y;
    float m_z;
    uint32_t abgr;
};

static PosColorVertex s_vertices[3] = {
    {-0.5, -0.5f, 0.0f, 0xFF4D00FF },
    { 0.5, -0.5f, 0.0f, 0xFF4D00FF },
    { 0.0, 0.5f, 0.0f, 0xFF4D00FF }
};

mg::VertexDecl s_decl;

#define VERT_HEAD \
"#version 440\n"
#define FRAG_HEAD \
"#version 440\n"

static const char* s_VertexShader = VERT_HEAD MOTI_TO_STRING(
    layout(location = 0) in vec3 a_position;
    layout(location = 1) in vec4 a_color;

    out vec4 color;
    void main() {
        color = a_color;
        gl_Position = vec4(a_position, 1.0);
    }
);

static const char* s_FragmentShader = FRAG_HEAD MOTI_TO_STRING(
    in vec4 color;
    out vec4 outColor;
    void main() {
        outColor = color;
    }
);
#include "moti/handle.h"

int main(int argc, char** argv) {
    
    moti::memory_globals::init();
   
    mem::StackAllocator<sizeof(s_vertices)> alloc;

    SDL_Init(SDL_INIT_VIDEO);
    moti::gl::GLContext context;
    SDL_Window* wnd = SDL_CreateWindow("moti", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
    context.create(wnd);
    glClearColor(0.f, 0.f, 0.f, 1.f);

    mg::GraphicsDevice device;

    moti::memory::StackAllocator<4096 * 2> shaderAlloc;
    mem::Block vertexShaderMem;
    mem::Block fragmentShaderMem;

    moti::MemoryWriter vshwriter(&vertexShaderMem, &shaderAlloc);

    uint32_t magic = MOTI_VERTEX_SHADER_MAGIC;
    moti::write<uint32_t>(&vshwriter, magic);
    int32_t len = static_cast<int32_t>(strlen(s_VertexShader));
    moti::write(&vshwriter, len);
    moti::write(&vshwriter, (void*)s_VertexShader, len);

    magic = MOTI_FRAGMENT_SHADER_MAGIC;
    moti::MemoryWriter fshwriter(&fragmentShaderMem, &shaderAlloc);
    moti::write<uint32_t>(&fshwriter, magic);
    len = static_cast<int32_t>(strlen(s_FragmentShader));
    moti::write(&fshwriter, len);
    moti::write(&fshwriter, (void*)s_FragmentShader, len);


    mg::ShaderHandle vsh = device.createShader(&vertexShaderMem);
    mg::ShaderHandle fsh = device.createShader(&fragmentShaderMem);
    mg::ProgramHandle p = device.createProgram(vsh, fsh);

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    mem::Block memory = alloc.allocate(sizeof(s_vertices));
    memcpy(memory.m_ptr, s_vertices, sizeof(s_vertices));

    s_decl.begin()
        .add(mg::Attribute::Position, 3, mg::AttributeType::Float, true)
        .add(mg::Attribute::Color, 4, mg::AttributeType::Uint8, false);

    mg::VertexBufferHandle vbo = device.createVertexBuffer(&memory, s_decl);
    device.setVertexBuffer(vbo);
    for (auto i = 0, index = 0; i < mg::Attribute::Count; ++i) {
        if (s_decl.m_attributes[i]) {
            glVertexAttribPointer(
                index,
                s_decl.m_count[i],
                (s_decl.m_type[i] == mg::AttributeType::Float) ? GL_FLOAT : GL_UNSIGNED_BYTE,
                !s_decl.m_normalized[i], 
                s_decl.m_stride,
                (void*)(uintptr_t)s_decl.m_offset[i]
                );
            glEnableVertexAttribArray(index++);
        }
    }

    glBindVertexArray(0);

    SDL_Event e;
    while (SDL_WaitEvent(&e)) {
        if (e.type == SDL_QUIT) break;
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(3);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glUseProgram(0);
        glBindVertexArray(0);
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