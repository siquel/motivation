#include "moti/renderer/renderer_gl.h"
#include "moti/memory/block.h"
#include "moti/io/io.h"
#include "moti/math/math.h"

namespace moti {
    namespace graphics {
        namespace gl {

            static const GLenum s_attribTypes[] = {
                GL_UNSIGNED_BYTE, // uint8
                GL_FLOAT          // float
            };

            static_assert(MOTI_COUNTOF(s_attribTypes) == AttributeType::Count, "Invalid amount of attribute types");

            RendererContextGL::RendererContextGL() {
                int versionMajor;
                int versionMinor;
                GL_CHECK(glGetIntegerv(GL_MAJOR_VERSION, &versionMajor));
                GL_CHECK(glGetIntegerv(GL_MAJOR_VERSION, &versionMinor));
                MOTI_TRACE("OpenGL context version: %d.%d", versionMajor, versionMinor);
                memset(m_vaos, 0, sizeof(m_vaos));
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
                m_programs[_handle.m_id].destroy();
            }

            void RendererContextGL::submit(ProgramHandle _handle, const Render& _draw) {
                GLProgram& program = m_programs[_handle.m_id];
                GLuint vao = m_vaos[program.m_id];
                if (vao == 0) {
                    GL_CHECK(glGenVertexArrays(1, &vao));
                    m_vaos[program.m_id] = vao;
                    GL_CHECK(glBindVertexArray(vao));
                    if (isValid(_draw.m_vertexBuffer)) {
                        GLVertexBuffer& vb = m_vertexBuffers[_draw.m_vertexBuffer.m_id];
                        GL_CHECK(glBindBuffer(vb.m_target, vb.m_id));
                        VertexDecl& decl = m_vertexDecls[vb.m_decl.m_id];
                        program.bindAttributes(decl);
                    }
                }
                else {
                    GL_CHECK(glBindVertexArray(vao));
                }
                // Enable depth test
                glEnable(GL_DEPTH_TEST);
                // Accept fragment if it closer to the camera than the former one
                glDepthFunc(GL_LESS);


                GLIndexBuffer& ibo = m_indexBuffers[_draw.m_indexBuffer.m_id];
                GL_CHECK(glUseProgram(program.m_id));
                GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo.m_id));

                GLint loc = glGetUniformLocation(program.m_id, "u_modelViewProj");
                
                moti::Mat4 view;
                moti::look(view, Vec3{ 0.f, 2.f, 0.f }, Vec3{ 0.f, 0.f, -4.f }, Vec3{ 0.f, 1.f, 0.f });
                moti::Mat4 projection;
                moti::perspective(projection, 45.f, 1.f * 1280 / 720, 0.1f, 10.f);
                moti::Mat4 model;
                model.setIdentity();
                translate(model, Vec3{ 0.f, 0.f, -4.f });
                float angle = SDL_GetTicks() / 1000.0f * 45.f;  // 45° per second
                
                moti::rotate(model, moti::radians(angle), Vec3{ 0.f, 1.f, 0.f });
                moti::Mat4 mvp2 = projection * view * model;
                glUniformMatrix4fv(loc, 1, GL_FALSE, toPointer(mvp2));
                
                GL_CHECK(glDrawElements(GL_TRIANGLES, _draw.m_indexCount, GL_UNSIGNED_SHORT, nullptr));
                GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
                GL_CHECK(glUseProgram(0));
            }

            void RendererContextGL::createVertexDecl(VertexDeclHandle _handle, const VertexDecl& _decl) {
                VertexDecl* decl = &m_vertexDecls[_handle.m_id];
                memcpy(decl, &_decl, sizeof(VertexDecl));
            }

            void RendererContextGL::createIndexBuffer(IndexBufferHandle _handle, mem::Block* _mem) {
                m_indexBuffers[_handle.m_id].create(_mem->m_length, _mem->m_ptr);
            }

            void GLVertexBuffer::create(uint32_t _size, void* _data, VertexDeclHandle _handle) {
                m_size = _size;
                m_decl = _handle;
                GL_CHECK(glGenBuffers(1, &m_id));
                MOTI_ASSERT(m_id != 0, "Failed to create vertex buffer");
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

                GLint numActiveAttribs{ 0 };
                GLint numActiveUniforms{ 0 };

                GL_CHECK(glGetProgramInterfaceiv(m_id, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &numActiveAttribs));
                GL_CHECK(glGetProgramInterfaceiv(m_id, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numActiveUniforms));

                glGetProgramiv(m_id, GL_ACTIVE_UNIFORMS, &numActiveUniforms);
                GLint max0, max1;
                GL_CHECK(glGetProgramiv(m_id, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max0));
                GL_CHECK(glGetProgramiv(m_id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max1));
                uint32_t nameMaxLength = std::max(max0, max1);
                char* name = static_cast<char*>(alloca(nameMaxLength + 1));

                MOTI_TRACE("Program %d", m_id);
                MOTI_TRACE("Attribute count %d", numActiveAttribs);

                for (int32_t i = 0; i < numActiveAttribs; ++i) {
                    GLint size;
                    GLenum type;
                    glGetActiveAttrib(m_id, i, nameMaxLength + 1, nullptr, &size, &type, name);
                    MOTI_TRACE("%s is at location %d", name, glGetAttribLocation(m_id, name));
                }

                MOTI_TRACE("Uniform count %d", numActiveUniforms);

                for (int32_t i = 0; i < numActiveUniforms; ++i) {
                    struct Info {
                        GLenum type;
                        GLint loc;
                        GLint count;
                    };

                    Info info;
                    GLenum props[] = { GL_TYPE, GL_LOCATION, GL_ARRAY_SIZE };

                    GL_CHECK(glGetProgramResourceiv(
                        m_id,
                        GL_UNIFORM,
                        i,
                        MOTI_COUNTOF(props), props, MOTI_COUNTOF(props),
                        NULL,
                        (GLint*)&info));

                    GL_CHECK(glGetProgramResourceName(
                        m_id,
                        GL_UNIFORM,
                        i,
                        nameMaxLength + 1,
                        NULL,
                        name));

                    uint16_t count = std::max(info.count, 1);

                    PredefinedUniform::Enum predef = nameToPredefinedUniform(name);
                    if (predef != PredefinedUniform::Count) {
                        m_predefinedUniforms[uniformCount].m_type = uint8_t(predef);
                        m_predefinedUniforms[uniformCount].m_loc = info.loc;
                        m_predefinedUniforms[uniformCount].m_count = count;
                        uniformCount++;
                    }
                    else {
                        // its user defined uniform
                    }

                    MOTI_TRACE("uniform %s is at location %d, count %d", name, info.loc, count);

                }

                memset(m_attributes, 0xff, sizeof(m_attributes));
                uint32_t used(0);
                for (uint8_t i = 0; i < Attribute::Count; ++i) {
                    GLint loc = glGetAttribLocation(m_id, s_attribNames[i]);
                    if (loc != -1) {
                        MOTI_TRACE("attribute %s: %d", s_attribNames[i], loc);
                        m_attributes[i] = loc;
                        m_used[used++] = i;
                    }
                }
                MOTI_ASSERT(used < MOTI_COUNTOF(m_used), "Out of bounds %d > array size %d", used, MOTI_COUNTOF(m_used));
                m_used[used] = Attribute::Count;
            }

            void GLProgram::bindAttributes(const VertexDecl& _decl) {

                for (uint32_t i = 0; Attribute::Count != m_used[i]; ++i) {
                    Attribute::Enum attr = Attribute::Enum(m_used[i]);
                    GLint loc = m_attributes[attr];

                    if (loc != -1) {
                        if (_decl.m_attributes[attr]) {
                            AttributeType::Enum type = _decl.m_type[attr];

                            GL_CHECK(glEnableVertexAttribArray(loc));
                            
                            GL_CHECK(glVertexAttribPointer(loc,
                                _decl.m_count[attr],
                                s_attribTypes[type],
                                !_decl.m_normalized[attr],
                                _decl.m_stride,
                                (void*)(uintptr_t)_decl.m_offset[attr]));
                        }
                    }
                }
            }

            void GLProgram::destroy() {
                if (m_id != 0) {
                    GL_CHECK(glUseProgram(0));
                    GL_CHECK(glDeleteProgram(m_id));
                    m_id = 0;
                }
            }

            void GLIndexBuffer::create(uint32_t _size, void* _data) {
                m_size = _size;
                GL_CHECK(glGenBuffers(1, &m_id));
                MOTI_ASSERT(m_id != 0, "Failed to create index buffer");
                GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id));
                GL_CHECK(glBufferData(
                    GL_ELEMENT_ARRAY_BUFFER,
                    m_size,
                    _data,
                    GL_STATIC_DRAW)
                    );
                GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
            }

        }
    }
}