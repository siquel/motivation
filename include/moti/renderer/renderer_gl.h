#pragma once

#include "moti/renderer/gl_context.h"
#include "moti/renderer/renderer_context.h"
#include "moti/memory/block.h"
#include <GL/glew.h>
#include "moti/config.h"

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

namespace moti {
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

            void create(Block* _mem);
            void destroy();
        };

        struct GLTexture {
            GLuint m_id;
            GLenum m_type;
            GLenum m_target;
            GLenum m_format;
            uint32_t m_width;
            uint32_t m_height;
            GLTexture()
                : m_id(0), m_type(GL_ZERO), m_target(GL_TEXTURE_2D), m_format(GL_ZERO), m_width(0), m_height(0) {}
            void init(GLenum target, uint32_t width, uint32_t height);
            void create(Block& memory);
            void destroy();
        };

        struct GLProgram {

            GLProgram()
                : m_id(0), m_uniforms(memory_globals::defaultAllocator()) {

            }
            GLuint m_id;

            uint8_t m_used[Attribute::Count + 1];
            GLint m_attributes[Attribute::Count];

            PredefinedUniform m_predefinedUniforms[PredefinedUniform::Count];
            uint8_t m_uniformCount{ 0 };
            Array<UniformDecl> m_uniforms;
            void create(const GLShader& _vsh, const GLShader& _fsh);
            void bindAttributes(const VertexDecl& _decl);
            void destroy();
        };

        struct RendererContextGL : public RendererContext {
            GLVertexBuffer m_vertexBuffers[MOTI_MAX_VERTEX_BUFFERS];
            GLIndexBuffer m_indexBuffers[MOTI_MAX_VERTEX_BUFFERS];
            GLShader m_shaders[MOTI_MAX_SHADERS];
            GLProgram m_programs[MOTI_MAX_PROGRAMS];
            // TODO this isn't working properly
            GLuint m_vaos[MOTI_MAX_PROGRAMS];
            VertexDecl m_vertexDecls[MOTI_MAX_VERTEX_BUFFERS];
            Block m_uniforms[MOTI_MAX_UNIFORMS];
            UniformRegistry m_uniformReg;
            GLTexture m_textures[MOTI_MAX_TEXTURES];

            RendererContextGL();
            ~RendererContextGL() override;
            virtual void createVertexBuffer(VertexBufferHandle _handle, Block* _mem, VertexDeclHandle _decl) override;
            virtual void destroyVertexBuffer(VertexBufferHandle _handle) override;
            virtual void createIndexBuffer(IndexBufferHandle _handle, Block* _mem) override;
            virtual void destroyIndexBuffer(IndexBufferHandle _handle) override;
            virtual void setVertexBuffer(VertexBufferHandle _handle) override;
            virtual void createShader(ShaderHandle _handle, Block* _mem) override;
            virtual void createProgram(ProgramHandle _handle, ShaderHandle _vertex, ShaderHandle _fragment) override;
            virtual void destroyShader(ShaderHandle _handle) override;
            virtual void destroyProgram(ProgramHandle _handle) override;
            virtual void createVertexDecl(VertexDeclHandle _handle, const VertexDecl& _decl) override;
            virtual void destroyVertexDecl(VertexDeclHandle _handle) override;
            virtual void submit(ProgramHandle _handle, const Render& _draw) override;
            virtual void createUniform(UniformHandle _handle, UniformType::Enum _type, uint16_t _count, const char* _name) override;
            virtual void destroyUniform(UniformHandle _handle) override;
            virtual void updateUniform(UniformHandle _handle, const void* _value, uint32_t _size) override;
            virtual void createTexture(TextureHandle handle, Block* memory) override;
            virtual void destroyTexture(TextureHandle handle) override;
            void setShaderUniform4f(uint32_t _index, const void* _val, uint32_t _num);
            void setShaderUniform4x4f(uint32_t _index, const void* _val, uint32_t _num);

            void setPredefined(const GLProgram& _program, const Render& _draw);
        };

        RendererContext* createRenderer();
        void destroyRenderer();
    }
}