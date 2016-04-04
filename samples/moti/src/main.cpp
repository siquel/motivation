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

struct POD {
    uint32_t top;
    uint32_t kek;
};
#define VERT_HEAD \
"#version 440\n"
#define FRAG_HEAD \
"#version 440\n"

static const char* s_VertexShader = VERT_HEAD MOTI_TO_STRING(
    layout(location = 0) in vec3 inPosition;
    layout(location = 1) in vec4 inColor;

    out vec4 color;
    void main() {
        color = inColor;
        gl_Position = vec4(inPosition, 1.0);
    }
);

static const char* s_FragmentShader = FRAG_HEAD MOTI_TO_STRING(
    in vec4 color;
    out vec4 outColor;
    void main() {
        outColor = color;
    }
);


int main(int argc, char** argv) {
    moti::memory_globals::init();

    const size_t size = sizeof(s_vertices);  

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
    glClearColor(0.f, 0.f, 0.f, 1.f);

    mg::GraphicsDevice device;


    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &s_VertexShader, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &s_FragmentShader, NULL);
    glCompileShader(fragmentShader);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    mg::VertexBufferHandle vbo = device.createVertexBuffer(&memory, s_decl);
    device.setVertexBuffer(vbo);
    for (auto i = 0, index = 0; i < mg::Attribute::Count; ++i) {
        if (s_decl.m_attributes[i]) {
            glVertexAttribPointer(
                index++,
                s_decl.m_count[i],
                (s_decl.m_type[i] == mg::AttributeType::Float) ? GL_FLOAT : GL_UNSIGNED_BYTE,
                !s_decl.m_normalized[i], 
                s_decl.m_stride,
                (void*)(uintptr_t)s_decl.m_offset[i]
                );
        }
    }
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    SDL_Event e;
    while (SDL_WaitEvent(&e)) {
        if (e.type == SDL_QUIT) break;
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glUseProgram(0);
        glBindVertexArray(0);
        SDL_GL_SwapWindow(wnd);
    }

    SDL_DestroyWindow(wnd);
    SDL_Quit();

    moti::memory_globals::shutdown();

    return 0;
}