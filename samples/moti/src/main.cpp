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
};

struct Light {
    moti::Vec4 m_position;
    moti::Vec3 m_ambient;
    moti::Vec3 m_diffuse;
    moti::Vec3 m_specular;
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


struct Textures {
    void init() {
        m_specular = load_texture("assets/container2_specular.png");
        m_diffuse = load_texture("assets/container2.png");
        m_emission = load_texture("assets/emission.png");
    }
    moti::TextureHandle m_specular;
    moti::TextureHandle m_diffuse; 
    moti::TextureHandle m_emission; 
};

static Textures s_textures;

struct Uniforms {

    void init() {
        using namespace moti;

        m_constantLinearQuadratic = { 1.0f, 0.09f, 0.032f };

        u_materialAmbient = moti::createUniform(moti::UniformType::Vec3, 1, "u_ambient");
        u_materialDiffuse = moti::createUniform(moti::UniformType::Vec3, 1, "u_diffuse");
        u_materialSpecular = moti::createUniform(moti::UniformType::Vec3, 1, "u_specular");

        u_lightPos = moti::createUniform(UniformType::Vec4, 1, "u_lightPos");
        u_lightAmbient = moti::createUniform(moti::UniformType::Vec3, 1, "u_lightAmbient");
        u_lightDiffuse = moti::createUniform(moti::UniformType::Vec3, 1, "u_lightDiffuse");
        u_lightSpecular = moti::createUniform(moti::UniformType::Vec3, 1, "u_lightSpecular");

        u_specularTexture = moti::createUniform(UniformType::Int1, 1, "u_specularTexture");
        u_diffuseTexture = moti::createUniform(UniformType::Int1, 1, "u_diffuseTexture");
        u_emissionTexture = moti::createUniform(UniformType::Int1, 1, "u_emissionTexture");

        u_constantLinearQuadratic = moti::createUniform(moti::UniformType::Vec3, 1, "u_constantLinearQuadratic");
    }

    void submitPerFrame() {
        moti::setUniform(u_lightPos, &m_light->m_position);
            moti::setUniform(u_lightAmbient, &m_light->m_ambient);
            moti::setUniform(u_lightDiffuse, &m_light->m_diffuse);
            moti::setUniform(u_lightSpecular, &m_light->m_specular);
        
        moti::setUniform(u_constantLinearQuadratic, &m_constantLinearQuadratic);
        moti::setTexture(0, u_specularTexture, s_textures.m_specular);
        moti::setTexture(1, u_diffuseTexture, s_textures.m_diffuse);
        moti::setTexture(2, u_emissionTexture, s_textures.m_emission);
    }

    moti::Vec3 m_constantLinearQuadratic;
    Light* m_light;
private:
    moti::UniformHandle u_materialAmbient;
    moti::UniformHandle u_materialDiffuse;
    moti::UniformHandle u_materialSpecular;

    moti::UniformHandle u_lightPos;
    moti::UniformHandle u_lightAmbient;
    moti::UniformHandle u_lightDiffuse;
    moti::UniformHandle u_lightSpecular;

    moti::UniformHandle u_specularTexture;
    moti::UniformHandle u_diffuseTexture;
    moti::UniformHandle u_emissionTexture;

    moti::UniformHandle u_constantLinearQuadratic;
};

static Uniforms s_uniforms;


int main(int argc, char** argv) {
    using namespace moti;

    moti::memory_globals::init();

    SDL_Init(SDL_INIT_VIDEO);
    moti::gl::GLContext context;
    SDL_Window* wnd = SDL_CreateWindow("moti", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Width, Height, SDL_WINDOW_OPENGL);
    context.create(wnd);
    glClearColor(0.0f, 0.f, 0.f, 1.f);

    moti::init();
    s_textures.init();
    s_uniforms.init();
    
    Light pointLight;
    pointLight.m_position = { 2.f, 1.f, -4.f, 1.f}; // non directional
    pointLight.m_ambient = { 0.1f, 0.1f, 0.1f };
    pointLight.m_diffuse = { 0.5f, 0.5f, 0.5f };
    pointLight.m_specular = { 1.0f, 1.0f, 1.0f };
    s_uniforms.m_light = &pointLight;

    moti::UniformHandle u_lightPos = moti::createUniform(UniformType::Vec4, 1, "u_lightPos");
    moti::UniformHandle u_time = moti::createUniform(UniformType::Float, 1, "u_time");

    moti::ProgramHandle p = load_program("shaders/palikka.vs", "shaders/point_light.fs");
    moti::ProgramHandle basic = load_program("shaders/basic.vs", "shaders/basic.fs");
    
    moti::Mat4 view;
    moti::look(view, Vec3{ 0.0f, 1.0f, 0.0f }, Vec3{ 0.0f, 1.0f, -4.f }, Vec3{ 0.f, 1.f, 0.f });
    moti::Mat4 projection;
    moti::perspective(projection, 45.f, float(Width) / float(Height), 0.1f, 100.f);

    Mesh mesh;
    mesh.load("cube.dae");

    Mesh lampMesh;
    lampMesh.load("cube.dae");

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    //glFrontFace(GL_CCW);
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);

    while (!s_exit) {
        pump_events();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        s_uniforms.submitPerFrame();

        float time = SDL_GetTicks() / 1000.f;

        float angle = time * 25.f;  // 45° per second
        moti::setUniform(u_time, &time);
        moti::setViewRect(0, 0, Width, Height);
        moti::setViewTransform(view, projection);

        moti::Mat4 model;
        model.setIdentity();
        translate(model, Vec3{ 0.f, -0.f, -6.f });
        moti::rotate(model, moti::radians(angle), Vec3{ 0.f, 1.f, 0.f });

        mesh.submit(p, model);

        model.setIdentity();
        {
            auto& pos = pointLight.m_position;
            translate(model, Vec3{ pos.x, pos.y, pos.z });
        }
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