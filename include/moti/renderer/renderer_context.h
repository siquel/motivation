#pragma once

#include "moti/moti.h"
#include "moti/math/math_types.h"
#include "../memory/block.h"
#include <unordered_map>
#include "../core/container/array.h"
#include "../memory/memory.h"
#include "../core/string/fixed_string.h"
#include <xhash>

MOTI_FORWARD_DECLARE_STRUCT(moti, Block);

namespace moti {
	

        static const uint8_t AttributeSizeGL[AttributeType::Count][4] = {
            { 1, 2, 4, 4}, // uint8
            { 4, 8, 12, 16} // float
        };

        static const uint32_t s_uniformTypeSize[] = {
            sizeof(int), //int1
            sizeof(float), // float
            sizeof(float) * 3, // vec3
            sizeof(float) * 4, // vec4
            sizeof(float) * 3 * 3, // mat3,
            sizeof(float) * 4 * 4 // mat4
        };

        static_assert(MOTI_COUNTOF(s_uniformTypeSize) == UniformType::Count, "Invalid amount of uniform type sizes");

        static uint16_t VertexDeclIdGenerator = 0;

		struct VertexDecl {
			uint16_t m_stride;
            uint16_t m_offset[Attribute::Count];
            uint8_t m_count[Attribute::Count];
            AttributeType::Enum m_type[Attribute::Count];
            bool m_attributes[Attribute::Count];
            bool m_normalized[Attribute::Count];

            uint16_t id;

            VertexDecl() {}

            inline bool has(Attribute::Enum _attr) { return m_attributes[_attr]; }

            VertexDecl& begin() {
                m_stride = 0;
                memset(&m_attributes, 0, sizeof(m_attributes));
                memset(&m_count, 0, sizeof(m_count));
                memset(&m_normalized, 0, sizeof(m_normalized));
                memset(&m_type, 0, sizeof(m_type));
                memset(&m_offset, 0, sizeof(m_offset));
                id = VertexDeclIdGenerator++;
                return *this;
            }

            VertexDecl& add(Attribute::Enum _attribute, uint8_t _count, AttributeType::Enum _type, bool _normalized) {
                m_offset[_attribute] = m_stride;
                m_stride += AttributeSizeGL[_type][_count - 1];
                m_attributes[_attribute] = true;
                m_count[_attribute] = _count;
                m_type[_attribute] = _type;
                m_normalized[_attribute] = _normalized;
                return *this;
            }
		};

		struct VertexBufferHandle {
			uint16_t m_id;
		};

        struct IndexBufferHandle {
            uint16_t m_id;
        };

		struct VertexDeclHandle {
			uint16_t m_id;
		};

        struct ShaderHandle {
            uint16_t m_id;
        };

        struct ProgramHandle {
            uint16_t m_id;
        };

        struct UniformHandle {
            uint16_t m_id;
        };

        struct TextureHandle {
            uint16_t m_id;
        };

		inline bool isValid(VertexBufferHandle _h) { return _h.m_id != UINT16_MAX; }
        inline bool isValid(IndexBufferHandle _h) { return _h.m_id != UINT16_MAX; }
		inline bool isValid(VertexDeclHandle _h) { return _h.m_id != UINT16_MAX; }
        inline bool isValid(ShaderHandle _h) { return _h.m_id != UINT16_MAX; }
        inline bool isValid(ProgramHandle _h) { return _h.m_id != UINT16_MAX; }
        inline bool isValid(TextureHandle _h) { return _h.m_id != UINT16_MAX; }
        
        struct Rect {
            uint32_t m_x;
            uint32_t m_y;
            uint32_t m_width;
            uint32_t m_height;
        };

        struct Uniform {
            UniformType::Enum m_type;
            uint16_t m_count;
        };

        struct UniformInfo {
            Block m_data;
            UniformHandle m_handle;
        };


        struct hasher {
            size_t operator() (const FixedString& s) const {
                size_t seed = 0;
                for (uint32_t i = 0; i < s.m_length; ++i) {
                    seed ^= std::hash_value(s.data() + i) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
                }
                return seed;
            }
        };

        class UniformRegistry {
        public:
            UniformRegistry() = default;
            ~UniformRegistry() = default;
            const UniformInfo* find(const char* _name) const {
                auto search = m_uniforms.find(FixedString(_name));
                if (search != m_uniforms.end()) {
                    return &search->second;
                }
                return nullptr;
            }

            const UniformInfo& add(UniformHandle _handle, const char* _name, Block _data) {
                UniformHashMap::iterator search = m_uniforms.find(FixedString(_name));
                if (search == m_uniforms.end()) {
                    UniformInfo info{ _data, _handle };
                    auto it = m_uniforms.insert(std::make_pair(FixedString(_name), info));
                    return it.first->second;
                }

                UniformInfo& info = search->second;
                info.m_data = _data;
                info.m_handle = _handle;
                return info;
            }
        private:
            using UniformHashMap = std::unordered_map<FixedString, UniformInfo, hasher>;
            UniformHashMap m_uniforms;
        };

        
        struct UniformDecl {
            UniformHandle m_handle;
            UniformType::Enum m_type;
            uint16_t m_loc;
            uint16_t m_count;
        };
        
        struct TextureBinding {
            uint16_t m_id;
        };

        struct Render {
            VertexBufferHandle m_vertexBuffer;
            IndexBufferHandle m_indexBuffer;
            uint32_t m_startVertex;
            uint32_t m_endVertex;
            uint32_t m_startIndex;
            uint32_t m_indexCount;
            Mat4 m_view;
            Mat4 m_proj;
            Rect m_viewRect;
            Mat4 m_model;
            TextureBinding m_bindings[8];

            void reset() {
                m_vertexBuffer.m_id = UINT16_MAX;
                m_indexBuffer.m_id = UINT16_MAX;
                m_startVertex = 0;
                m_endVertex = UINT32_MAX;
                m_startIndex = 0;
                m_indexCount = UINT32_MAX;
                memset(m_bindings, UINT16_MAX, sizeof(m_bindings));
//              m_view.setIdentity();
//              m_proj.setIdentity();
//              memset(&m_viewRect, 0, sizeof(Rect));
            }
        };

		struct RendererContext {
			virtual ~RendererContext() = 0;
			virtual void createVertexBuffer(VertexBufferHandle _handle, Block* _mem, VertexDeclHandle _decl) = 0;
            virtual void destroyVertexBuffer(VertexBufferHandle _handle) = 0;
            virtual void createIndexBuffer(IndexBufferHandle _handle, Block* _mem) = 0;
            virtual void destroyIndexBuffer(IndexBufferHandle _handle) = 0;
            virtual void setVertexBuffer(VertexBufferHandle _handle) = 0;
            virtual void createShader(ShaderHandle _handle, Block* _mem) = 0; 
            virtual void createProgram(ProgramHandle _handle, ShaderHandle _vertex, ShaderHandle _fragment) = 0;
            virtual void destroyShader(ShaderHandle _handle) = 0;
            virtual void destroyProgram(ProgramHandle _handle) = 0;
            virtual void createVertexDecl(VertexDeclHandle _handle, const VertexDecl& _decl) = 0;
            virtual void destroyVertexDecl(VertexDeclHandle _handle) = 0;
            virtual void createUniform(UniformHandle _handle, UniformType::Enum _type, uint16_t _count, const char* _name) = 0;
            virtual void destroyUniform(UniformHandle _handle) = 0;
            virtual void updateUniform(UniformHandle _handle, const void* _value, uint32_t _size) = 0;
            virtual void createTexture(TextureHandle handle, Block* memory) = 0;
            virtual void destroyTexture(TextureHandle handle) = 0;
            virtual void submit(ProgramHandle _handle, const Render& _draw) = 0;
		};
		inline RendererContext::~RendererContext() {}
	
}