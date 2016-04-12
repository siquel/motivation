#pragma once

#include "moti/moti.h"
#include "moti/math/math_types.h"

MOTI_FORWARD_DECLARE_STRUCT(moti, memory, Block);

namespace moti {
	namespace mem = memory;
	namespace graphics {

        static const uint8_t AttributeSizeGL[AttributeType::Count][4] = {
            { 1, 2, 4, 4}, // uint8
            { 4, 8, 12, 16} // float
        };

        static uint16_t VertexDeclIdGenerator = 0;

		struct VertexDecl {
			uint16_t m_stride;
            uint16_t m_offset[Attribute::Count];
            uint8_t m_count[Attribute::Count];
            AttributeType::Enum m_type[Attribute::Count];
            bool m_attributes[Attribute::Count];
            bool m_normalized[Attribute::Count];

            uint16_t id;

            VertexDecl() : id(VertexDeclIdGenerator++) {}

            inline bool has(Attribute::Enum _attr) { return m_attributes[_attr]; }

            VertexDecl& begin() {
                memset(&m_attributes, 0, sizeof(m_attributes));
                memset(&m_count, 0, sizeof(m_count));
                memset(&m_normalized, 0, sizeof(m_normalized));
                memset(&m_type, 0, sizeof(m_type));
                memset(&m_offset, 0, sizeof(m_offset));
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

		inline bool isValid(VertexBufferHandle _h) { return _h.m_id != UINT16_MAX; }
        inline bool isValid(IndexBufferHandle _h) { return _h.m_id != UINT16_MAX; }
		inline bool isValid(VertexDeclHandle _h) { return _h.m_id != UINT16_MAX; }
        inline bool isValid(ShaderHandle _h) { return _h.m_id != UINT16_MAX; }
        inline bool isValid(ProgramHandle _h) { return _h.m_id != UINT16_MAX; }
        
        struct Rect {
            uint32_t m_x;
            uint32_t m_y;
            uint32_t m_width;
            uint32_t m_height;
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

            void reset() {
                m_vertexBuffer.m_id = UINT16_MAX;
                m_indexBuffer.m_id = UINT16_MAX;
                m_startVertex = 0;
                m_endVertex = UINT32_MAX;
                m_startIndex = 0;
                m_indexCount = UINT32_MAX;
                m_view.setIdentity();
                m_proj.setIdentity();
                memset(&m_viewRect, 0, sizeof(Rect));
            }
        };

		struct RendererContext {
			virtual ~RendererContext() = 0;
			virtual void createVertexBuffer(VertexBufferHandle _handle, mem::Block* _mem, VertexDeclHandle _decl) = 0;
            virtual void createIndexBuffer(IndexBufferHandle _handle, mem::Block* _mem) = 0;
            virtual void setVertexBuffer(VertexBufferHandle _handle) = 0;
            virtual void createShader(ShaderHandle _handle, mem::Block* _mem) = 0; 
            virtual void createProgram(ProgramHandle _handle, ShaderHandle _vertex, ShaderHandle _fragment) = 0;
            virtual void destroyShader(ShaderHandle _handle) = 0;
            virtual void destroyProgram(ProgramHandle _handle) = 0;
            virtual void createVertexDecl(VertexDeclHandle _handle, const VertexDecl& _decl) = 0;
            virtual void submit(ProgramHandle _handle, const Render& _draw) = 0;
		};
		inline RendererContext::~RendererContext() {}
	}
}