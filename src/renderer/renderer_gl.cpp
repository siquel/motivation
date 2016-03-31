#include "moti/renderer/renderer_gl.h"
#include "moti/memory/block.h"

namespace moti {
	namespace graphics {
		namespace gl {

			RendererContextGL::RendererContextGL() {

			}

			RendererContextGL::~RendererContextGL() {

			}

			void RendererContextGL::createVertexBuffer(VertexBufferHandle _handle, mem::Block* _mem, VertexDeclHandle _decl) {
				m_vertexBuffers[_handle.m_id].create(_mem->m_length, _mem->m_ptr, _decl);
			}

            void RendererContextGL::setVertexBuffer(VertexBufferHandle _handle) {
                auto& buffer = m_vertexBuffers[_handle.m_id];
                GL_CHECK(glBindBuffer(buffer.m_target, buffer.m_id));
            }

			void GLVertexBuffer::create(uint32_t _size, void* _data, VertexDeclHandle _handle) {
				m_size = _size;
				m_decl = _handle;
				GL_CHECK(glGenBuffers(1, &m_id));
				assert(m_id != 0);
				m_target = GL_ARRAY_BUFFER;
				GL_CHECK(glBindBuffer(m_target, m_id));
				GL_CHECK(glBufferData(m_target, m_size, _data, (_data == nullptr) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
				GL_CHECK(glBindBuffer(m_target, 0));
			}
		}
	}
}