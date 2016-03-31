#pragma once

#include "moti/renderer/gl_context.h"
#include "moti/renderer/renderer_context.h"
#include <GL/glew.h>

#if _DEBUG
#define GL_CHECK(_call)                            \
	do {                                           \
	   _call;                                      \
	   GLenum glerr = glGetError();                \
	   assert(glerr == GL_NO_ERROR);               \
    } while (0)
#else
#define GL_CHECK(_call) _call
#endif

#define MAX_VERTEX_BUFFERS 4 << 10

namespace moti {
	namespace graphics {
		namespace gl {



			struct GLVertexBuffer {
				GLuint m_id;
				GLenum m_target;
				uint32_t m_size;
				VertexDeclHandle m_decl;

				void create(uint32_t _size, void* _data, VertexDeclHandle _handle);
			};

			struct RendererContextGL : public graphics::RendererContext {
				GLVertexBuffer m_vertexBuffers[MAX_VERTEX_BUFFERS];

				RendererContextGL();
				~RendererContextGL() override;
				virtual void createVertexBuffer(VertexBufferHandle _handle, mem::Block* _mem, VertexDeclHandle _decl) override;
                virtual void setVertexBuffer(VertexDeclHandle _handle) override;
			};

		}
	}
}