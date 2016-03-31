#pragma once

#include "moti/moti.h"
MOTI_FORWARD_DECLARE_STRUCT(moti, memory, Block);

namespace moti {
	namespace mem = memory;
	namespace graphics {

		struct Attribute {
			enum Enum {
				Position,
				Color,
				Count
			};
		};

		struct AttributeType {
			enum Enum {
				Uint8,
				Float,
				Count
			};
		};

        static const uint8_t AttributeSizeGL[AttributeType::Count][4] = {
            { 1, 2, 4, 4}, // uint8
            { 4, 8, 12, 16} // float
        };

        static uint16_t VertexDeclIdGenerator = 0;

		struct VertexDecl {
			uint16_t m_stride;
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
                return *this;
            }

            VertexDecl& add(Attribute::Enum _attribute, uint8_t _count, AttributeType::Enum _type, bool _normalized) {
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

		struct VertexDeclHandle {
			uint16_t m_id;
		};

		inline bool isValid(VertexBufferHandle _h) { return _h.m_id != UINT16_MAX; }
		inline bool isValid(VertexDeclHandle _h) { return _h.m_id != UINT16_MAX; }

		struct RendererContext {
			virtual ~RendererContext() = 0;
			virtual void createVertexBuffer(VertexBufferHandle _handle, mem::Block* _mem, VertexDeclHandle _decl) = 0;
            virtual void setVertexBuffer(VertexDeclHandle _handle) = 0;
		};
		inline RendererContext::~RendererContext() {}
	}
}