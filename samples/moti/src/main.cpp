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

static bool s_exit = false;

struct Material {
    moti::Vec3 m_ambient;
    moti::Vec3 m_diffuse;
    moti::Vec3 m_specular;
    float m_shininess;
};

void pump_events() {
    static SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) s_exit = true;
    }
}

struct MaterialUniforms {
    moti::UniformHandle m_ambient;
    moti::UniformHandle m_diffuse;
    moti::UniformHandle m_specular;
    moti::UniformHandle m_shininess;
};

static MaterialUniforms s_material;


int main(int argc, char** argv) {
    using namespace moti;

    moti::memory_globals::init();

    SDL_Init(SDL_INIT_VIDEO);
    moti::gl::GLContext context;
    SDL_Window* wnd = SDL_CreateWindow("moti", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Width, Height, SDL_WINDOW_OPENGL);
    context.create(wnd);
    glClearColor(0.0f, 1.f, 0.f, 1.f);

    moti::init();
    
    moti::TextureHandle texture = load_texture("assets/vittu.png");
    moti::TextureHandle texture2 = load_texture("assets/ella.png");
    moti::UniformHandle u_textureSampler = moti::createUniform(UniformType::Int1, 1, "u_texture");
    moti::UniformHandle u_textureSampler2 = moti::createUniform(UniformType::Int1, 1, "u_texture2");

    moti::UniformHandle u_lightPos = moti::createUniform(UniformType::Vec3, 1, "u_lightPos");
    moti::UniformHandle u_time = moti::createUniform(UniformType::Float, 1, "u_time");
    s_material.m_ambient = moti::createUniform(moti::UniformType::Vec3, 1, "u_ambient");
    s_material.m_diffuse = moti::createUniform(moti::UniformType::Vec3, 1, "u_diffuse");
    s_material.m_specular = moti::createUniform(moti::UniformType::Vec3, 1, "u_specular");
    s_material.m_shininess = moti::createUniform(moti::UniformType::Float, 1, "u_shininess");

    moti::UniformHandle u_lightAmbient = moti::createUniform(moti::UniformType::Vec3, 1, "u_lightAmbient");
    moti::UniformHandle u_lightDiffuse = moti::createUniform(moti::UniformType::Vec3, 1, "u_lightDiffuse");
    moti::UniformHandle u_lightSpecular = moti::createUniform(moti::UniformType::Vec3, 1, "u_lightSpecular");

    {
        float vec[3] = { 1.f, 1.f, 1.f };
        moti::setUniform(u_lightAmbient, vec);
        moti::setUniform(u_lightDiffuse, vec);
        moti::setUniform(u_lightSpecular, vec);
    }

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

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    //glFrontFace(GL_CCW);
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);

    Material emerald;
    emerald.m_ambient = { 0.0f, 0.1f, 0.06f };
    emerald.m_diffuse = { 0.0f, 0.50980392f, 0.50980392f };
    emerald.m_specular = { 0.50196078f, 0.50196078f, 0.50196078f };
    emerald.m_shininess = 32.f;

    // material stuff
    //////////////////////////////////////////////////////////////////////////
    moti::setUniform(s_material.m_ambient, &emerald.m_ambient);
    moti::setUniform(s_material.m_diffuse, &emerald.m_diffuse);
    moti::setUniform(s_material.m_specular, &emerald.m_specular);
    moti::setUniform(s_material.m_shininess, &emerald.m_shininess);
    //////////////////////////////////////////////////////////////////////////


    while (!s_exit) {
        pump_events();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float time = SDL_GetTicks() / 1000.f;

        float angle = time * 25.f;  // 45° per second
       


        moti::setUniform(u_lightPos, &lamp_pos);
        moti::setUniform(u_time, &time);
        moti::setViewRect(0, 0, Width, Height);
        moti::setViewTransform(view, projection);
        moti::setTexture(0, u_textureSampler, texture);
        moti::setTexture(1, u_textureSampler2, texture2);
        moti::Mat4 model;
        model.setIdentity();
        translate(model, Vec3{ 0.f, -0.f, -6.f });
        moti::rotate(model, moti::radians(angle), Vec3{ 0.f, 1.f, 0.f });


        mesh.submit(p, model);

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