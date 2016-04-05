#include "moti/renderer/graphics_device.h"
#include "moti/renderer/renderer_gl.h"
#include "moti/io/io.h"
#include "moti/handle.h"
namespace moti {
    namespace graphics {

        HandleManager<4096> m_vertexBufferHandles;
        HandleManager<128> m_shaderHandles;
        HandleManager<128> m_programHandles;

        GraphicsDevice::GraphicsDevice()
            : m_ctx(new gl::RendererContextGL) {

        }
        GraphicsDevice::~GraphicsDevice() {
            delete m_ctx;
        }

        VertexBufferHandle GraphicsDevice::createVertexBuffer(mem::Block* _mem, const VertexDecl& _decl) {
            VertexBufferHandle handle{ m_vertexBufferHandles.create() };
            if (isValid(handle)) {
                VertexDeclHandle declhandle;
                if (m_declLookup.find(_decl.id) == std::end(m_declLookup)) {
                    declhandle = { _decl.id };
                    m_declLookup[_decl.id] = _decl;
                }
                m_ctx->createVertexBuffer(handle, _mem, declhandle);
            }
            return handle;
        }

        void GraphicsDevice::setVertexBuffer(VertexBufferHandle _handle) {
            m_ctx->setVertexBuffer(_handle);
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

    }
}