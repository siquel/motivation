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
				TexCoord0,
				Normal,
				Tangent,
				Bitangent,
				Count
			};
		};

		struct AttributeType {
			enum Enum {
				Uint8,
				Int16,
				Float,
				Count
			};
		};

		struct VertexDecl {
			uint16_t m_stride;
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
		};
		inline RendererContext::~RendererContext() {}
	}
}