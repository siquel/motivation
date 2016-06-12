#pragma once

#include "moti/moti.h"
#include <stdio.h>
#include <stdarg.h>
#include "moti/renderer/graphics_device.h"
#include "moti/io/io.h"
#include "moti/memory/stack_allocator.h"

namespace moti {

    static char* s_deviceMem[sizeof(GraphicsDevice)];
    static GraphicsDevice* s_device = nullptr;

    static const char* s_predefinedUniformNames[] = {
        "u_viewRect",
        "u_view",
        "u_proj",
        "u_viewProj",
        "u_model",
        "u_modelViewProj"
    };

    static_assert(MOTI_COUNTOF(s_predefinedUniformNames) == PredefinedUniform::Count, "Uniform names size differs from enum");

    int32_t vsnprintf(char* _str, size_t _count, const char* _format, va_list _argList) {
        int32_t len = ::vsnprintf_s(_str, _count, size_t(-1), _format, _argList);
        return -1 == len ? ::_vscprintf(_format, _argList) : len;
    }

    void trace(const char* _path, uint16_t _line, const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        char temp[8192];
        char* out = temp;
        int32_t len = moti::vsnprintf(out, sizeof(temp), fmt, args);
        out[len] = '\0';
        va_end(args);
        debugOutput(out);
    }

    void abort(const char* path, uint16_t line, const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);
       
    }

    void fatal(const char* format, ...) {
        moti::debugBreak();
    }

    PredefinedUniform::Enum nameToPredefinedUniform(const char* _name) {
        for (uint32_t i = 0; i < MOTI_COUNTOF(s_predefinedUniformNames); ++i) {
            if (strcmp(_name, s_predefinedUniformNames[i]) == 0) {
                return PredefinedUniform::Enum(i);
            }
        }
        return PredefinedUniform::Count;
    }

    const char* predefinedUniformName(PredefinedUniform::Enum _enum) {
        return s_predefinedUniformNames[_enum];
    }

    void init()
    {
        MOTI_ASSERT(s_device == nullptr, "Device has already been initialized");
        s_device = new (s_deviceMem)GraphicsDevice();
    }

    moti::VertexBufferHandle createVertexBuffer(Block* _mem, const VertexDecl& _decl)
    {
        MOTI_ASSERT(s_device != nullptr, "Device hasnt been initialized");
        return s_device->createVertexBuffer(_mem, _decl);
    }

    moti::IndexBufferHandle createIndexBuffer(Block* _mem)
    {
        MOTI_ASSERT(s_device != nullptr, "Device hasnt been initialized");
        return s_device->createIndexBuffer(_mem);
    }

    void setVertexBuffer(VertexBufferHandle _handle, uint32_t _startVertex, uint32_t _count)
    {
        MOTI_ASSERT(s_device != nullptr, "Device hasnt been initialized");
        s_device->setVertexBuffer(_handle, _startVertex, _count);
    }

    void setIndexBuffer(IndexBufferHandle _handle, uint32_t firstIndex, uint32_t _count)
    {
        MOTI_ASSERT(s_device != nullptr, "Device hasnt been initialized");
        s_device->setIndexBuffer(_handle, firstIndex, _count);
    }

    moti::ShaderHandle createShader(Block* _mem)
    {
        MOTI_ASSERT(s_device != nullptr, "Device hasnt been initialized");
        return s_device->createShader(_mem);
    }

    moti::ProgramHandle createProgram(ShaderHandle _vertex, ShaderHandle _fragment)
    {
        MOTI_ASSERT(s_device != nullptr, "Device hasnt been initialized");
        return s_device->createProgram(_vertex, _fragment);
    }

    void destroyVertexBuffer(VertexBufferHandle _handle)
    {
        MOTI_ASSERT(s_device != nullptr, "Device hasnt been initialized");
        s_device->destroyVertexBuffer(_handle);
    }

    void destroyIndexBuffer(IndexBufferHandle _handle)
    {
        MOTI_ASSERT(s_device != nullptr, "Device hasnt been initialized");
        s_device->destroyIndexBuffer(_handle);
    }

    moti::UniformHandle createUniform(UniformType::Enum _type, uint16_t _count, const char* _name)
    {
        MOTI_ASSERT(s_device != nullptr, "Device hasnt been initialized");
        return s_device->createUniform(_type, _count, _name);
    }

    void destroyUniform(UniformHandle _handle)
    {
        MOTI_ASSERT(s_device != nullptr, "Device hasnt been initialized");
        s_device->destroyUniform(_handle);
    }

    void destroyShader(ShaderHandle _handle)
    {
        MOTI_ASSERT(s_device != nullptr, "Device hasnt been initialized");
        s_device->destroyShader(_handle);
    }

    void destroyProgram(ProgramHandle _handle)
    {
        MOTI_ASSERT(s_device != nullptr, "Device hasnt been initialized");
        return s_device->destroyProgram(_handle);
    }

    void submit(ProgramHandle _program)
    {
        MOTI_ASSERT(s_device != nullptr, "Device hasnt been initialized");
        s_device->submit(_program);
    }

    void setViewTransform(const Mat4& _view, const Mat4& _proj)
    {
        MOTI_ASSERT(s_device != nullptr, "Device hasnt been initialized");
        s_device->setViewTransform(_view, _proj);
    }

    void setViewRect(uint32_t _x, uint32_t _y, uint32_t _w, uint32_t _h)
    {
        MOTI_ASSERT(s_device != nullptr, "Device hasnt been initialized");
        s_device->setViewRect(_x, _y, _w, _h);
    }

    void setTransform(const Mat4& _mtx)
    {
        MOTI_ASSERT(s_device != nullptr, "Device hasnt been initialized");
        s_device->setTransform(_mtx);
    }

    void setUniform(UniformHandle _handle, const void* _value)
    {
        MOTI_ASSERT(s_device != nullptr, "Device hasnt been initialized");
        s_device->setUniform(_handle, _value);
    }

    moti::TextureHandle createTexture(uint16_t width, uint16_t height, Block* memory)
    {
        StackAllocator<128> alloc;
        uint32_t size = sizeof(TextureHeader);
        Block block = alloc.allocate(size);
        StaticMemoryWriter writer(block.m_ptr, block.m_length);

        TextureHeader h;
        h.m_width = width;
        h.m_height = height;
        h.m_sides = 0;
        h.m_cubemap = false;
        h.m_ptr = memory;

        write(&writer, h);

        return s_device->createTexture(&block);
    }

    void setTexture(uint8_t unit, UniformHandle sampler, TextureHandle texture)
    {
        MOTI_ASSERT(s_device != nullptr, "Device hasnt been initialized");
        s_device->setTexture(unit, sampler, texture);
    }

}
