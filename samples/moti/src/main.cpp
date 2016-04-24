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
    layout(location = 1) in vec3 a_normal;
    layout(location = 2) in vec2 a_texCoord0;
    uniform float u_time;

    out vec3 v_pos;
    out vec3 v_view;
    out vec3 v_normal;
    out vec4 v_color;
    void main() {
        vec3 pos = a_position;

        float sx = sin(pos.x*32.0 + u_time.x*4.0)*0.5 + 0.5;
        float cy = cos(pos.y*32.0 + u_time.x*4.0)*0.5 + 0.5;
        vec3 displacement = vec3(sx, cy, sx*cy);
        vec3 normal = a_normal.xyz*2.0 - 1.0;

        pos = pos + normal*displacement*vec3(0.06, 0.06, 0.06);

        gl_Position = u_modelViewProj * vec4(pos, 1.0);

        mat4 mv = u_view * u_model;
        v_pos = gl_Position.xyz;
        v_view = (mv * vec4(pos, 1.0)).xyz;

        v_normal = (mv * vec4(normal, 0.0)).xyz;
        
        float len = length(displacement)*0.4 + 0.6;
        v_color = vec4(len, len, len, 1.0);
    }
);

static const char* s_FragmentShader = VERT_HEAD "\n#define M_PI 3.1415926535897935\n" MOTI_TO_STRING(
    in vec3 v_pos;
    in vec3 v_view;
    in vec3 v_normal;
    in vec4 v_color;
    out vec4 outcolor;
    uniform float u_time;

    vec2 blinn(vec3 _lightDir, vec3 _normal, vec3 _viewDir)
    {
        float ndotl = dot(_normal, _lightDir);
        vec3 reflected = _lightDir - 2.0*ndotl*_normal; // reflect(_lightDir, _normal);
        float rdotv = dot(reflected, _viewDir);
        return vec2(ndotl, rdotv);
    }

    float fresnel(float _ndotl, float _bias, float _pow)
    {
        float facing = (1.0 - _ndotl);
        return max(_bias + (1.0 - _bias) * pow(facing, _pow), 0.0);
    }

    vec4 lit(float _ndotl, float _rdotv, float _m)
    {
        float diff = max(0.0, _ndotl);
        float spec = step(0.0, _ndotl) * max(0.0, _rdotv * _m);
        return vec4(1.0, diff, spec, 1.0);
    }

    void main()
    {
        vec3 lightDir = vec3(0.0, 0.0, -1.0);
        vec3 normal = normalize(v_normal);
        vec3 view = normalize(v_view);
        vec2 bln = blinn(lightDir, normal, view);
        vec4 lc = lit(bln.x, bln.y, 1.0);
        float fres = fresnel(bln.x, 0.2, 5.0);

        float index = ((sin(v_pos.x*3.0 + u_time)*0.3 + 0.7)
            + (cos(v_pos.y*3.0 + u_time)*0.4 + 0.6)
            + (cos(v_pos.z*3.0 + u_time)*0.2 + 0.8)
            )*M_PI;

        vec3 color = vec3(sin(index*8.0)*0.4 + 0.6
            , sin(index*4.0)*0.4 + 0.6
            , sin(index*2.0)*0.4 + 0.6
            ) * v_color.xyz;

        outcolor.xyz = (pow(vec3(0.07, 0.06, 0.08) + color*lc.y + fres*pow(lc.z, 128.0), vec3(1.0 / 2.2)));
        outcolor.w = 1.0;
        
    }
);

mg::ShaderHandle createShader(const char* src, uint32_t magic, mg::GraphicsDevice& device) {
    moti::memory::StackAllocator<4096> shaderAlloc;
    mem::Block block;
    moti::MemoryWriter writer(&block, &shaderAlloc);
    moti::write<uint32_t>(&writer, magic);
    int32_t len = static_cast<int32_t>(strlen(src));
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

    mg::UniformHandle u_time = device.createUniform(UniformType::Float, 1, "u_time");
    MOTI_ASSERT(u_time.m_id == device.createUniform(UniformType::Float, 1, "u_time").m_id, "Handles are different");

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
    moti::look(view, Vec3{ 0.0f, 1.0f, 0.0f }, Vec3{ 0.0f, 1.0f, -3.f }, Vec3{ 0.f, 1.f, 0.f });
    moti::Mat4 projection;
    moti::perspective(projection, 45.f, float(Width) / float(Height), 0.1f, 100.f);

    Mesh mesh;
    mesh.load("bunny.obj", &device);

    SDL_Event e;
    bool running = true;

    while (running) {

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float time = SDL_GetTicks() / 1000.f;
        float angle = time * 45.f;  // 45° per second

        device.setUniform(u_time, &time);
        device.setViewRect(0, 0, Width, Height);
        device.setViewTransform(view, projection);

        moti::Mat4 model;
        model.setIdentity();
        translate(model, Vec3{ 0.f, 0.f, -4.f });
        moti::rotate(model, moti::radians(angle), Vec3{ 0.f, 1.f, 0.f });

        mesh.submit(device, p, model);
        /*
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
                device.setUniform(u_time, &time);
                device.submit(p);
            }
        }*/
        
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

    return 0;
}