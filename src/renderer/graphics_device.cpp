#include "..\..\include\moti\renderer\graphics_device.h"
#include "..\..\include\moti\renderer\graphics_device.h"
#include "moti/renderer/graphics_device.h"
#include "moti/renderer/renderer_gl.h"
#include "moti/io/io.h"
#include "moti/handle.h"
namespace moti {
    namespace graphics {

        HandleManager<4096> m_vertexBufferHandles;
        HandleManager<4096> m_indexBufferHandles;
        HandleManager<128> m_shaderHandles;
        HandleManager<128> m_programHandles;

        GraphicsDevice::GraphicsDevice()
            : m_ctx(new gl::RendererContextGL) {
            m_draw.reset();
        }
        GraphicsDevice::~GraphicsDevice() {
            delete m_ctx;
        }

        VertexBufferHandle GraphicsDevice::createVertexBuffer(mem::Block* _mem, const VertexDecl& _decl) {
            VertexBufferHandle handle{ m_vertexBufferHandles.create() };
            if (isValid(handle)) {
                VertexDeclHandle declhandle{ UINT16_MAX };
                if (m_declLookup.find(_decl.id) == std::end(m_declLookup)) {
                    declhandle = { _decl.id };
                    m_declLookup[_decl.id] = _decl;
                    m_ctx->createVertexDecl(declhandle, _decl);
                }
                m_ctx->createVertexBuffer(handle, _mem, declhandle);
            }
            return handle;
        }

        IndexBufferHandle GraphicsDevice::createIndexBuffer(mem::Block* _mem) {
            IndexBufferHandle handle{ m_indexBufferHandles.create() };
            if (isValid(handle)) {
                m_ctx->createIndexBuffer(handle, _mem);
            }
            return handle;
        }

        void GraphicsDevice::setVertexBuffer(VertexBufferHandle _handle, uint32_t _startVertex, uint32_t _count) {
            m_draw.m_vertexBuffer = _handle;
            m_draw.m_startVertex = _startVertex;
            m_draw.m_endVertex = _count;
        }

        void GraphicsDevice::setIndexBuffer(IndexBufferHandle _handle, uint32_t firstIndex, uint32_t _count) {
            m_draw.m_indexBuffer = _handle;
            m_draw.m_startIndex = firstIndex;
            m_draw.m_indexCount = _count;
        }

        ShaderHandle GraphicsDevice::createShader(mem::Block* _mem) {
            uint32_t magic = 0;
            moti::MemoryReader reader(_mem->m_ptr, _mem->m_length);
            moti::read(&reader, magic);

            if (magic != MOTI_FRAGMENT_SHADER_MAGIC && magic != MOTI_VERTEX_SHADER_MAGIC) {
                return ShaderHandle{ UINT16_MAX };
            }
            ShaderHandle handle{ m_shaderHandles.create() };
            if (isValid(handle)) {
                m_ctx->createShader(handle, _mem);
            }
            return handle;
        }

        ProgramHandle GraphicsDevice::createProgram(ShaderHandle _vertex, ShaderHandle _fragment) {
            if (!isValid(_vertex) || !isValid(_fragment)) {
                MOTI_TRACE("Invalid vertex / fragment shaders: vsh: %d, fsh: %d", _vertex.m_id, _fragment.m_id);
                return ProgramHandle{ UINT16_MAX };
            }
            ProgramHandle handle = ProgramHandle{ m_programHandles.create() };
            if (isValid(handle)) {
                m_ctx->createProgram(handle, _vertex, _fragment);
            }
            return handle;
        }

        void GraphicsDevice::destroyShader(ShaderHandle _handle) {
            if (isValid(_handle)) {
                m_ctx->destroyShader(_handle);
            }
        }

        void GraphicsDevice::destroyProgram(ProgramHandle _handle) {
            if (isValid(_handle)) {
                m_ctx->destroyProgram(_handle);
            }
        }

        void GraphicsDevice::submit(ProgramHandle _program) {
            m_ctx->submit(_program, m_draw);
            m_draw.reset();
        }

    }
}