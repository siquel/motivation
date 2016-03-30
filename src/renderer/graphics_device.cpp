#include "moti/renderer/graphics_device.h"
#include "moti/renderer/renderer_gl.h"

namespace moti {
    namespace graphics {

        GraphicsDevice::GraphicsDevice()
            : m_ctx(new gl::RendererContextGL) {

        }
        GraphicsDevice::~GraphicsDevice() {
            delete m_ctx;
        }
        VertexBufferHandle GraphicsDevice::createVertexBuffer(mem::Block* _mem, const VertexDecl& _decl) {
            return VertexBufferHandle{};
        }
    }
}