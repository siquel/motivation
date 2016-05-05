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
#include "util.h"
namespace mem = moti::memory;
namespace mg = moti::graphics;

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

    mg::UniformHandle u_time = device.createUniform(UniformType::Float, 1, "u_time");

    mg::ProgramHandle p = load_program("shaders/juttu.vs", "shaders/juttu.fs", device);
    
    moti::Mat4 view;
    moti::look(view, Vec3{ 0.0f, 1.0f, 0.0f }, Vec3{ 0.0f, 1.0f, -4.f }, Vec3{ 0.f, 1.f, 0.f });
    moti::Mat4 projection;
    moti::perspective(projection, 45.f, float(Width) / float(Height), 0.1f, 100.f);

    Mesh mesh;
    mesh.load("cube.obj", &device);

    SDL_Event e;
    bool running = true;

    while (running) {

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float time = SDL_GetTicks() / 1000.f;
        float angle = time * 25.f;  // 45° per second

        device.setUniform(u_time, &time);
        device.setViewRect(0, 0, Width, Height);
        device.setViewTransform(view, projection);

        moti::Mat4 model;
        model.setIdentity();
        translate(model, Vec3{ 0.f, -0.f, -5.f });
        //moti::rotate(model, moti::radians(angle), Vec3{ 0.f, 1.f, 0.f });
        glFrontFace(GL_CCW);
        glEnable(GL_CULL_FACE);
        //glCullFace(GL_FRONT);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        mesh.submit(device, p, model);
        
        SDL_GL_SwapWindow(wnd);
    }

    device.destroyUniform(u_time);
    device.destroyProgram(p);
    SDL_DestroyWindow(wnd);
    SDL_Quit();

    return 0;
}