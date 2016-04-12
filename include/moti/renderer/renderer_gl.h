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
#define MAX_INDEX_BUFFERS MAX_VERTEX_BUFFERS
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

            struct GLIndexBuffer {
                GLuint m_id;
                uint32_t m_size;
                void create(uint32_t _size, void* data);
            };

            struct GLShader {
                GLuint m_id;
                GLenum m_type;

                void create(mem::Block* _mem);
                void destroy();
            };

            struct GLProgram {
                GLuint m_id{ 0 };
                
                uint8_t m_used[Attribute::Count + 1];
                GLint m_attributes[Attribute::Count];

                PredefinedUniform m_predefinedUniforms[PredefinedUniform::Count];
                uint8_t m_uniformCount{ 0 };
                void create(const GLShader& _vsh, const GLShader& _fsh);
                void bindAttributes(const VertexDecl& _decl);
                void destroy();
            };

			struct RendererContextGL : public graphics::RendererContext {
				GLVertexBuffer m_vertexBuffers[MAX_VERTEX_BUFFERS];
                GLIndexBuffer m_indexBuffers[MAX_INDEX_BUFFERS];
                GLShader m_shaders[MAX_SHADERS];
                GLProgram m_programs[MAX_PROGRAMS];
                // TODO this isn't working properly
                GLuint m_vaos[MAX_PROGRAMS];
                VertexDecl m_vertexDecls[MAX_VERTEX_BUFFERS];

				RendererContextGL();
				~RendererContextGL() override;
				virtual void createVertexBuffer(VertexBufferHandle _handle, mem::Block* _mem, VertexDeclHandle _decl) override;
                virtual void destroyVertexBuffer(VertexBufferHandle _handle) override;
                virtual void createIndexBuffer(IndexBufferHandle _handle, mem::Block* _mem) override;
                virtual void destroyIndexBuffer(IndexBufferHandle _handle) override;
                virtual void setVertexBuffer(VertexBufferHandle _handle) override;
                virtual void createShader(ShaderHandle _handle, mem::Block* _mem) override;
                virtual void createProgram(ProgramHandle _handle, ShaderHandle _vertex, ShaderHandle _fragment) override;
                virtual void destroyShader(ShaderHandle _handle) override;
                virtual void destroyProgram(ProgramHandle _handle) override;
                virtual void createVertexDecl(VertexDeclHandle _handle, const VertexDecl& _decl) override;
                virtual void destroyVertexDecl(VertexDeclHandle _handle) override;
                virtual void submit(ProgramHandle _handle, const Render& _draw) override;
                virtual void createUniform(UniformHandle _handle, UniformType::Enum _type, uint16_t _count, const char* _name) override;
                virtual void destroyUniform(UniformHandle _handle) override;
                void setShaderUniform4f(uint32_t _index, const void* _val, uint32_t _num);
                void setShaderUniform4x4f(uint32_t _index, const void* _val, uint32_t _num);

                void setPredefined(const GLProgram& _program, const Render& _draw);
			};

		}
	}
}