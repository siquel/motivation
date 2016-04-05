#include "moti/renderer/graphics_device.h"
#include "moti/renderer/renderer_gl.h"

namespace moti {
    namespace graphics {

        static uint16_t VertexBufferCounter = 0;

        GraphicsDevice::GraphicsDevice()
            : m_ctx(new gl::RendererContextGL) {

        }
        GraphicsDevice::~GraphicsDevice() {
            delete m_ctx;
        }

        VertexBufferHandle GraphicsDevice::createVertexBuffer(mem::Block* _mem, const VertexDecl& _decl) {
            VertexBufferHandle handle{ VertexBufferCounter++ };
            VertexDeclHandle declhandle;
            if (m_declLookup.find(_decl.id) == std::end(m_declLookup)) {
                declhandle = { _decl.id };
                m_declLookup[_decl.id] = _decl;
            }
            m_ctx->createVertexBuffer(handle, _mem, declhandle);
            return handle;
        }

        void GraphicsDevice::setVertexBuffer(VertexBufferHandle _handle) {
            m_ctx->setVertexBuffer(_handle);
        }

        ShaderHandle GraphicsDevice::createShader(mem::Block* _mem) {
            return ShaderHandle{ UINT16_MAX };
        }

        ProgramHandle GraphicsDevice::createProgram(ShaderHandle _vertex, ShaderHandle _fragment) {
            return ProgramHandle{ UINT16_MAX };
        }

    }
}