#include "moti/moti.h"
#include "moti/memory/stack_allocator.h"
#include <assert.h>
#include <SDL/SDL.h>
#include "moti/renderer/gl_context.h"
#include "moti/memory/memory.h"
#include "moti/io/io.h"
#include "moti/math/math.h"
#include "util.h"

const int Width = 1280;
const int Height = 720;

int main(int argc, char** argv) {
    using moti::Attribute;
    using moti::AttributeType;
    using moti::Mat4;
    using moti::Vec3;
    using moti::UniformType;

    moti::memory_globals::init();
   
    moti::StackAllocator<4096> alloc;

    SDL_Init(SDL_INIT_VIDEO);
    moti::gl::GLContext context;
    SDL_Window* wnd = SDL_CreateWindow("moti", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Width, Height, SDL_WINDOW_OPENGL);
    context.create(wnd);
    glClearColor(0.0f, 0.f, 0.f, 1.f);

    moti::init();
    
    moti::TextureHandle texture = load_texture("assets/vittu.png");
    
    moti::UniformHandle u_time = moti::createUniform(UniformType::Float, 1, "u_time");
    moti::UniformHandle u_lightPos = moti::createUniform(UniformType::Vec4, 1, "u_lightPos");

    moti::ProgramHandle p = load_program("shaders/palikka.vs", "shaders/palikka.fs");
    moti::ProgramHandle basic = load_program("shaders/basic.vs", "shaders/basic.fs");
    
    moti::Mat4 view;
    moti::look(view, Vec3{ 0.0f, 1.0f, 0.0f }, Vec3{ 0.0f, 1.0f, -4.f }, Vec3{ 0.f, 1.f, 0.f });
    moti::Mat4 projection;
    moti::perspective(projection, 45.f, float(Width) / float(Height), 0.1f, 100.f);

    Mesh mesh;
    mesh.load("cube.dae");

    Mesh lampMesh;
    lampMesh.load("cube.dae");
    moti::Vec3 lamp_pos = { -2.f, 3.f, -4.f };

    SDL_Event e;
    bool running = true;


    while (running) {

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float time = SDL_GetTicks() / 1000.f;
        float angle = time * 25.f;  // 45° per second
        // TODO vec3 uniforms ":D"
        float lampdata[4] = { lamp_pos.x, lamp_pos.y, lamp_pos.z, 1.0f };
        moti::setUniform(u_lightPos, lampdata);
        //moti::setUniform(u_time, &time);
        moti::setViewRect(0, 0, Width, Height);
        moti::setViewTransform(view, projection);

        moti::Mat4 model;
        model.setIdentity();
        translate(model, Vec3{ 0.f, -0.f, -6.f });
        moti::rotate(model, moti::radians(angle), Vec3{ 0.f, 1.f, 0.f });
        //glFrontFace(GL_CCW);
        //glEnable(GL_CULL_FACE);
        //glCullFace(GL_BACK);
        glEnable(GL_DEPTH_TEST);
        //glDepthFunc(GL_LESS);
        glBindTexture(GL_TEXTURE_2D, 1);
        mesh.submit(p, model);
        glBindTexture(GL_TEXTURE_2D, 0);

        model.setIdentity();
        translate(model, lamp_pos);
        scale(model, Vec3{ 0.2f, 0.2f, 0.2f });
        lampMesh.submit(basic, model);

        SDL_GL_SwapWindow(wnd);
    }

    moti::destroyUniform(u_time);
    moti::destroyProgram(p);
    SDL_DestroyWindow(wnd);
    SDL_Quit();

    return 0;
}