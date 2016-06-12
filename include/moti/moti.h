#pragma once

#include "macros.h"
#include "platform.h"
#include <assert.h>
#include <stdint.h>
namespace moti {
    // http://cnicholson.net/2011/01/stupid-c-tricks-a-better-sizeof_array/
    template<typename T, size_t N> char(&COUNTOF_REQUIRES_ARRAY_ARGUMENT(const T(&)[N]))[N];
#define MOTI_COUNTOF(_x) sizeof(moti::COUNTOF_REQUIRES_ARRAY_ARGUMENT(_x) )
}

#define FOURCC(_a, _b, _c, _d) ( ( (uint32_t)(_a) | ( (uint32_t)(_b) << 8) | ( (uint32_t)(_c) << 16) | ( (uint32_t)(_d) << 24) ) )

#define MOTI_VERTEX_SHADER_MAGIC   FOURCC('V', 'S', 'H', 0x0)
#define MOTI_FRAGMENT_SHADER_MAGIC FOURCC('F', 'S', 'H', 0x0)

#if _DEBUG
#define _MOTI_TRACE(fmt, ...) for (;;) { moti::trace(__FILE__, uint16_t(__LINE__), fmt "\n", ##__VA_ARGS__); break;}
#define MOTI_TRACE _MOTI_TRACE


#define MOTI_ASSERT(cond, msg, ...)                                                                                       \
        do {                                                                                                              \
            if (!(cond)) {                                                                                                \
                MOTI_TRACE(msg, ##__VA_ARGS__);                                                                           \
                moti::abort(__FILE__, uint16_t(__LINE__), "\nAssertion failed: %s\n\t  " msg "\n", #cond, ##__VA_ARGS__); \
                assert(0 && #cond);                                                                                       \
            }                                                                                                             \
        } while(0)
#endif

#define MOTI_NOOP(...) for(;;) { break; }

#ifndef MOTI_TRACE
#    define MOTI_TRACE(fmt, ...) MOTI_NOOP()
#endif

#ifndef MOTI_ASSERT
#    define MOTI_ASSERT(cond, fmt, ...) MOTI_NOOP()
#endif

#if MOTI_PLATFORM_WINDOWS
#include <Windows.h>
#endif

#include "debug.h"

namespace moti {
    struct Block;
    struct TextureHeader {
        uint16_t m_width;
        uint16_t m_height;
        uint16_t m_sides;
        bool m_cubemap;
        Block* m_ptr;
    };

    struct UniformType {
        enum Enum {
            Int1,
            Float,
            Vec3,
            Vec4,
            Mat3,
            Mat4,
            Count
        };
    };

    struct PredefinedUniform {
        enum Enum {
            ViewRect,
            View,
            Proj,
            ViewProj,
            Model,
            ModelViewProj,
            Count
        };

        uint32_t m_loc;
        uint16_t m_count;
        uint8_t m_type;
    };

    struct Attribute {
        enum Enum {
            Position,
            Color,
            Normal,
            TexCoord0,
            TexCoord1,
            TexCoord2,
            TexCoord3,
            TexCoord4,
            TexCoord5,
            TexCoord6,
            TexCoord7,
            Count
        };
    };

    static const char* s_attribNames[] = {
        "a_position",
        "a_color",
        "a_normal",
        "a_texCoord0",
        "a_texCoord1",
        "a_texCoord2",
        "a_texCoord3",
        "a_texCoord4",
        "a_texCoord5",
        "a_texCoord6",
        "a_texCoord7"
    };
    static_assert(MOTI_COUNTOF(s_attribNames) == Attribute::Count, "Invalid amount of attribute names");

    struct AttributeType {
        enum Enum {
            Uint8,
            Float,
            Count
        };
    };

    int32_t vsnprintf(char* _str, size_t _count, const char* _format, va_list _argList);
    void trace(const char* path, uint16_t line, const char* fmt, ...);
    void abort(const char* path, uint16_t line, const char* fmt, ...);
    void fatal(const char* format, ...);

    PredefinedUniform::Enum nameToPredefinedUniform(const char* _name);

    const char* predefinedUniformName(PredefinedUniform::Enum _enum);


}


// "API"
namespace moti
{
    struct VertexBufferHandle;
    struct IndexBufferHandle;
    struct VertexDecl;
    struct ShaderHandle;
    struct ProgramHandle;
    struct UniformHandle;
    struct TextureHandle;
    struct Mat4;
    struct Block;
    
    void init();

    VertexBufferHandle createVertexBuffer(Block* _mem, const VertexDecl& _decl);
    IndexBufferHandle createIndexBuffer(Block* _mem);
    void setVertexBuffer(VertexBufferHandle _handle, uint32_t _startVertex, uint32_t _count);
    void setIndexBuffer(IndexBufferHandle _handle, uint32_t firstIndex, uint32_t _count);
    ShaderHandle createShader(Block* _mem);
    ProgramHandle createProgram(ShaderHandle _vertex, ShaderHandle _fragment);

    void destroyVertexBuffer(VertexBufferHandle _handle);
    void destroyIndexBuffer(IndexBufferHandle _handle);

    UniformHandle createUniform(UniformType::Enum _type, uint16_t _count, const char* _name);
    void destroyUniform(UniformHandle _handle);

    void destroyShader(ShaderHandle _handle);
    void destroyProgram(ProgramHandle _handle);
    void submit(ProgramHandle _program);

    void setViewTransform(const Mat4& _view, const Mat4& _proj);
    void setViewRect(uint32_t _x, uint32_t  _y, uint32_t _w, uint32_t h);
    void setTransform(const Mat4& _mtx);
    void setUniform(UniformHandle _handle, const void* _value);

    TextureHandle createTexture(uint16_t width, uint16_t height, Block* memory);
    TextureHandle createCubeMap(uint16_t width, uint16_t height, Block* memory);
    void updateCubeMap(TextureHandle handle, uint8_t side, uint16_t x, uint16_t y, uint16_t widht, uint16_t height, Block& memory);

    void setTexture(uint8_t unit, UniformHandle sampler, TextureHandle texture);
}