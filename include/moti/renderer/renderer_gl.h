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
#define MAX_SHADERS        512
#define MAX_PROGRAMS       128

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

            struct GLShader {
                GLuint m_id;
                GLenum m_type;

                void create(mem::Block* _mem);
                void destroy();
            };

            struct GLProgram {
                GLuint m_id;
                uint8_t m_used[Attribute::Count + 1];
                GLint m_attributes[Attribute::Count];
                void create(const GLShader& _vsh, const GLShader& _fsh);
                void destroy();
            };

			struct RendererContextGL : public graphics::RendererContext {
				GLVertexBuffer m_vertexBuffers[MAX_VERTEX_BUFFERS];
                GLShader m_shaders[MAX_SHADERS];
                GLProgram m_programs[MAX_PROGRAMS];

				RendererContextGL();
				~RendererContextGL() override;
				virtual void createVertexBuffer(VertexBufferHandle _handle, mem::Block* _mem, VertexDeclHandle _decl) override;
                virtual void setVertexBuffer(VertexBufferHandle _handle) override;
                virtual void createShader(ShaderHandle _handle, mem::Block* _mem) override;
                virtual void createProgram(ProgramHandle _handle, ShaderHandle _vertex, ShaderHandle _fragment) override;
                virtual void destroyShader(ShaderHandle _handle) override;
                virtual void destroyProgram(ProgramHandle _handle) override;
                virtual void submit(ProgramHandle _handle) override;
			};

		}
	}
}