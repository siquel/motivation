#include "moti/renderer/renderer_gl.h"
#include "moti/memory/block.h"
#include "moti/io/io.h"

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

            void RendererContextGL::createShader(ShaderHandle _handle, mem::Block* _mem) {
                m_shaders[_handle.m_id].create(_mem);
            }

            void RendererContextGL::createProgram(ProgramHandle _handle, ShaderHandle _vertex, ShaderHandle _fragment) {
                GLShader& vsh = m_shaders[_vertex.m_id];
                GLShader& fsh = m_shaders[_fragment.m_id];
                m_programs[_handle.m_id].create(vsh, fsh);
            }

            void RendererContextGL::destroyShader(ShaderHandle _handle) {
                m_shaders[_handle.m_id].destroy();
            }

            void RendererContextGL::destroyProgram(ProgramHandle _handle) {
            }

            void RendererContextGL::submit(ProgramHandle _handle) {
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

            void GLShader::create(mem::Block* _mem) {
                moti::MemoryReader reader(_mem->m_ptr, _mem->m_length);
                uint32_t magic(0);
                moti::read(&reader, magic);

                
                switch (magic) {
                case MOTI_VERTEX_SHADER_MAGIC: m_type = GL_VERTEX_SHADER; break;
                case MOTI_FRAGMENT_SHADER_MAGIC: m_type = GL_FRAGMENT_SHADER; break;
                default:
                    MOTI_ASSERT(0, "unkown shader format %x", magic);
                    break;
                }

                int32_t shaderSize;
                moti::read(&reader, shaderSize);

                m_id = glCreateShader(m_type);
                const char* src = reinterpret_cast<const char*>(reader.getPointer());

                GL_CHECK(glShaderSource(m_id, 1, (const GLchar**)&src, NULL));
                GL_CHECK(glCompileShader(m_id));

                GLint compiled(0);
                GL_CHECK(glGetShaderiv(m_id, GL_COMPILE_STATUS, &compiled));

                if (compiled == 0) {
                    GLsizei len;
                    char log[1024];
                    GL_CHECK(glGetShaderInfoLog(m_id, sizeof(log), &len, log));
                    MOTI_TRACE("Failed to compile shader %d: %s", compiled, log);
                    GL_CHECK(glDeleteShader(m_id));
                    m_id = 0;
                }
            }

            void GLShader::destroy() {
                if (m_id != 0) {
                    GL_CHECK(glDeleteShader(m_id));
                    m_id = 0;
                }
            }

            void GLProgram::create(const GLShader& _vsh, const GLShader& _fsh) {
                m_id = glCreateProgram();

                GLint linked(0);
                if (_vsh.m_id != 0) {
                    GL_CHECK(glAttachShader(m_id, _vsh.m_id));

                    if (_fsh.m_id != 0) {
                        GL_CHECK(glAttachShader(m_id, _fsh.m_id));
                    }

                    GL_CHECK(glLinkProgram(m_id));
                    GL_CHECK(glGetProgramiv(m_id, GL_LINK_STATUS, &linked));

                    if (!linked) {
                        char log[1024];
                        GL_CHECK(glGetProgramInfoLog(m_id, sizeof(log), NULL, log));
                        MOTI_TRACE("%d: %s", linked, log);
                    }
                }

                if (!linked) {
                    GL_CHECK(glDeleteProgram(m_id));
                    m_id = 0;
                    MOTI_TRACE("Invalid shaders: vsh %d, fsh %d", _vsh.m_id, _fsh.m_id);
                    return;
                }

                // TODO bind attributes
            }

            void GLProgram::destroy() {
                if (m_id != 0) {
                    GL_CHECK(glUseProgram(0));
                    GL_CHECK(glDeleteProgram(m_id));
                    m_id = 0;
                }
            }

        }
	}
}