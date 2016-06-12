#include "moti/renderer/renderer_gl.h"
#include "moti/memory/block.h"
#include "moti/io/io.h"
#include "moti/math/math.h"
#include "moti/memory/memory.h"

namespace moti {
    namespace gl {

        static RendererContextGL* s_ctx;
        static Block s_rendererMem;

        RendererContext* createRenderer() {
            if (!s_rendererMem) {
                s_rendererMem = memory_globals::defaultAllocator().allocate(sizeof(RendererContextGL));
                s_ctx = new (s_rendererMem.m_ptr) RendererContextGL;
            }
            return s_ctx;
        }

        void destroyRenderer() {
            memory_globals::defaultAllocator().deallocate(s_rendererMem);
            s_ctx = nullptr;
        }

        static const GLenum s_attribTypes[] = {
            GL_UNSIGNED_BYTE, // uint8
            GL_FLOAT          // float
        };

        static_assert(MOTI_COUNTOF(s_attribTypes) == AttributeType::Count, "Invalid amount of attribute types");

        UniformType::Enum glTypeToUniformType(GLenum _type) {
            switch (_type) {
            case GL_SAMPLER_2D:
                return UniformType::Int1;
            case GL_FLOAT:
                return UniformType::Float;
                /*                case GL_FLOAT_VEC2:*/
            case GL_FLOAT_VEC3:
                return UniformType::Vec3;
            case GL_FLOAT_VEC4:
                return UniformType::Vec4;
            case GL_FLOAT_MAT3:
                return UniformType::Mat3;
            case GL_FLOAT_MAT4:
                return UniformType::Mat4;
            }
            MOTI_ASSERT(false, "Unrecognized enum 0x%04x", _type);
            return UniformType::Count;
        }

        RendererContextGL::RendererContextGL() {
            int versionMajor;
            int versionMinor;
            GL_CHECK(glGetIntegerv(GL_MAJOR_VERSION, &versionMajor));
            GL_CHECK(glGetIntegerv(GL_MAJOR_VERSION, &versionMinor));
            MOTI_TRACE("OpenGL context version: %d.%d", versionMajor, versionMinor);
            memset(m_vaos, 0, sizeof(m_vaos));
            memset(m_uniforms, 0, sizeof(m_uniforms));
        }

        RendererContextGL::~RendererContextGL() {

        }

        void RendererContextGL::setShaderUniform4f(uint32_t _index, const void* _val, uint32_t _num) {
            GL_CHECK(glUniform4fv(_index, _num, (const GLfloat*)_val));
        }

        void RendererContextGL::setShaderUniform4x4f(uint32_t _index, const void* _val, uint32_t _num) {
            GL_CHECK(glUniformMatrix4fv(_index, _num, GL_FALSE, (const GLfloat*)_val));
        }

        void RendererContextGL::createVertexBuffer(VertexBufferHandle _handle, Block* _mem, VertexDeclHandle _decl) {
            GLuint vao = 0;
            GL_CHECK(glGenVertexArrays(1, &vao));
            m_vertexBuffers[_handle.m_id].create(_mem->m_length, _mem->m_ptr, _decl);
            m_vaos[_handle.m_id] = vao;
        }

        void RendererContextGL::destroyVertexBuffer(VertexBufferHandle _handle)
        {
        }

        void RendererContextGL::setVertexBuffer(VertexBufferHandle _handle) {
            auto& buffer = m_vertexBuffers[_handle.m_id];
            GL_CHECK(glBindBuffer(buffer.m_target, buffer.m_id));
        }

        void RendererContextGL::createShader(ShaderHandle _handle, Block* _mem) {
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
            GLIndexBuffer* ibo = nullptr;
            GLProgram& program = m_programs[_handle.m_id];
            GLuint vao = m_vaos[_draw.m_vertexBuffer.m_id];

            GL_CHECK(glBindVertexArray(vao));
            if (isValid(_draw.m_vertexBuffer)) {
                GLVertexBuffer& vb = m_vertexBuffers[_draw.m_vertexBuffer.m_id];
                GL_CHECK(glBindBuffer(vb.m_target, vb.m_id));
                VertexDecl& decl = m_vertexDecls[vb.m_decl.m_id];
                program.bindAttributes(decl);
            }

            //glActiveTexture(GL_TEXTURE0);

            const bool drawIndexed = _draw.m_indexBuffer.m_id != UINT16_MAX;

            if (drawIndexed) {
                ibo = &m_indexBuffers[_draw.m_indexBuffer.m_id];
                GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo->m_id));
            }

            GL_CHECK(glUseProgram(program.m_id));

            setPredefined(program, _draw);
            Array<UniformDecl>& uniforms = program.m_uniforms;

#define UNIFORM_IMPL_CASE(_uniform, _suffix, _type) \
                 case UniformType::_uniform:  \
                 {                   \
                     _type* value = (_type*)data;    \
                     glUniform##_suffix(uniform->m_loc, uniform->m_count, value); \
                 }\
                 break
            for (auto uniform = uniforms.begin(); uniform != uniforms.end(); ++uniform) {
                const char* data = (const char*)m_uniforms[uniform->m_handle.m_id].m_ptr;
                switch (uniform->m_type) {
                    UNIFORM_IMPL_CASE(Float, 1fv, float);
                    UNIFORM_IMPL_CASE(Vec3, 3fv, float);
                    UNIFORM_IMPL_CASE(Vec4, 4fv, float);
                    UNIFORM_IMPL_CASE(Int1, 1iv, int);
                case UniformType::Mat3:
                {
                    float* value = (float*)data;
                    GL_CHECK(glUniformMatrix4fv(uniform->m_loc, uniform->m_count, GL_FALSE, value));
                }
                break;
                case UniformType::Mat4:
                {
                    float* value = (float*)data;
                    GL_CHECK(glUniformMatrix4fv(uniform->m_loc, uniform->m_count, GL_FALSE, value));
                }
                break;
                }
            }
#undef UNIFORM_IMPL_CASE

            for (uint32_t unit = 0; unit < MOTI_COUNTOF(_draw.m_bindings); ++unit) {
                const TextureBinding& bind = _draw.m_bindings[unit];
                if (bind.m_id != UINT16_MAX) {
                    GLTexture& texture = m_textures[bind.m_id];

                    GL_CHECK(glActiveTexture(GL_TEXTURE0 + unit));
                    GL_CHECK(glBindTexture(texture.m_target, texture.m_id));
                }
            }

            if (drawIndexed) {
                GL_CHECK(glDrawElements(GL_TRIANGLES, _draw.m_indexCount, GL_UNSIGNED_SHORT, nullptr));
                GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
            }
            else {

                GL_CHECK(glDrawArrays(GL_TRIANGLES, _draw.m_startVertex, _draw.m_endVertex));
            }
            // unbind
            for (uint32_t unit = 0; unit < MOTI_COUNTOF(_draw.m_bindings); ++unit) {
                const TextureBinding& bind = _draw.m_bindings[unit];
                if (bind.m_id != UINT16_MAX) {
                    GLTexture& texture = m_textures[bind.m_id];
                    GL_CHECK(glActiveTexture(GL_TEXTURE0 + unit));
                    GL_CHECK(glBindTexture(texture.m_target, 0));
                }
            }

            GL_CHECK(glBindVertexArray(0));
            GL_CHECK(glUseProgram(0));
        }

        void RendererContextGL::createUniform(UniformHandle _handle, UniformType::Enum _type, uint16_t _count, const char * _name) {
            Block& data = m_uniforms[_handle.m_id];
            if (data) {
                memory_globals::defaultAllocator().deallocate(m_uniforms[_handle.m_id]);
            }
            uint32_t size = s_uniformTypeSize[_type] * _count;
            data = memory_globals::defaultAllocator().allocate(size);
            memset(data.m_ptr, 0, data.m_length);
            m_uniforms[_handle.m_id];
            m_uniformReg.add(_handle, _name, data);
        }

        void RendererContextGL::updateUniform(UniformHandle _handle, const void* _value, uint32_t _size) {
            memcpy(m_uniforms[_handle.m_id].m_ptr, _value, _size);
        }

        void RendererContextGL::createTexture(TextureHandle handle, Block * memory)
        {
            m_textures[handle.m_id].create(*memory);
        }

        void RendererContextGL::destroyTexture(TextureHandle handle)
        {
            m_textures[handle.m_id].destroy();
        }

        void RendererContextGL::destroyUniform(UniformHandle _handle)
        {
        }

        void RendererContextGL::createVertexDecl(VertexDeclHandle _handle, const VertexDecl& _decl) {
            VertexDecl* decl = &m_vertexDecls[_handle.m_id];
            memcpy(decl, &_decl, sizeof(VertexDecl));
        }

        void RendererContextGL::destroyVertexDecl(VertexDeclHandle _handle)
        {
        }

        void RendererContextGL::createIndexBuffer(IndexBufferHandle _handle, Block* _mem) {
            m_indexBuffers[_handle.m_id].create(_mem->m_length, _mem->m_ptr);
        }

        void RendererContextGL::destroyIndexBuffer(IndexBufferHandle _handle)
        {
        }

        void RendererContextGL::setPredefined(const GLProgram& _program, const Render& _draw) {

            for (uint32_t i = 0; i < _program.m_uniformCount; ++i) {
                const PredefinedUniform& predefined = _program.m_predefinedUniforms[i];
                switch (predefined.m_type) {
                case PredefinedUniform::ViewRect:
                    float frect[4];
                    frect[0] = (float)_draw.m_viewRect.m_x;
                    frect[1] = (float)_draw.m_viewRect.m_y;
                    frect[2] = (float)_draw.m_viewRect.m_width;
                    frect[3] = (float)_draw.m_viewRect.m_height;
                    setShaderUniform4f(predefined.m_loc, frect, predefined.m_count);
                    break;
                case PredefinedUniform::View:
                    setShaderUniform4x4f(predefined.m_loc, &_draw.m_view, predefined.m_count);
                    break;
                case PredefinedUniform::Proj:
                    setShaderUniform4x4f(predefined.m_loc, &_draw.m_proj, predefined.m_count);
                    break;
                case PredefinedUniform::ViewProj:
                    Mat4 vp = _draw.m_proj * _draw.m_view;
                    setShaderUniform4x4f(predefined.m_loc, &vp, predefined.m_count);
                case PredefinedUniform::Model:
                    setShaderUniform4x4f(predefined.m_loc, &_draw.m_model, predefined.m_count);
                    break;
                case PredefinedUniform::ModelViewProj:
                    Mat4 mvp = _draw.m_proj * _draw.m_view * _draw.m_model;
                    setShaderUniform4x4f(predefined.m_loc, &mvp, predefined.m_count);
                    break;
                }
            }
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

        void GLShader::create(Block* _mem) {
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
                    m_predefinedUniforms[m_uniformCount].m_type = uint8_t(predef);
                    m_predefinedUniforms[m_uniformCount].m_loc = info.loc;
                    m_predefinedUniforms[m_uniformCount].m_count = count;
                    m_uniformCount++;
                }
                else {
                    // its user defined uniform
                    const UniformInfo* uniformInfo = s_ctx->m_uniformReg.find(name);
                    if (uniformInfo == nullptr) {
                        MOTI_TRACE("User defined uniform %s is not found", name);
                    }
                    else {
                        UniformType::Enum type = glTypeToUniformType(info.type);
                        UniformDecl decl;
                        decl.m_handle = uniformInfo->m_handle;
                        decl.m_loc = info.loc;
                        decl.m_count = count;
                        decl.m_type = type;
                        m_uniforms.push_back(decl);
                    }
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

        void GLTexture::init(GLenum target, uint32_t width, uint32_t height)
        {
            m_width = width;
            m_height = height;
            m_target = target;
            GL_CHECK(glGenTextures(1, &m_id));
            MOTI_ASSERT(m_id != 0, "Failed to generate texture id");
            GL_CHECK(glBindTexture(target, m_id));

            m_format = GL_RGBA;

        }

        void GLTexture::create(Block& memory)
        {
            MemoryReader reader(memory.m_ptr, memory.m_length);
            moti::TextureHeader header;
            read(&reader, header);

            GLenum target = GL_TEXTURE_2D;
            
            if (header.m_cubemap)
            {
                target = GL_TEXTURE_CUBE_MAP;
            }


            init(target, header.m_width, header.m_height);

            m_type = GL_UNSIGNED_BYTE;
            
            void* data = nullptr;

            if (header.m_ptr) {
                data = header.m_ptr->m_ptr;
            }


            if (m_target == GL_TEXTURE_2D) {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                GL_CHECK(glTexImage2D(m_target, 0, m_format, header.m_width, header.m_height, 0, m_format, m_type, data));
            }
            else if (m_target == GL_TEXTURE_CUBE_MAP) {
                MOTI_ASSERT(false, "Not implemented");
            }

            GL_CHECK(glBindTexture(m_target, 0));
        }

        void GLTexture::destroy()
        {
            if (m_id != 0) {
                GL_CHECK(glBindTexture(m_target, 0));
                GL_CHECK(glDeleteTextures(1, &m_id));
                m_id = 0;
            }
        }

    }

}