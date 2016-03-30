#pragma once

#include "moti/renderer/renderer_context.h"
#include <unordered_map>
namespace moti {
	namespace graphics {
		class GraphicsDevice {
        private:
            RendererContext* m_ctx;
            // TODO
            std::unordered_map<uint32_t, VertexDecl> m_declLookup;
        public:
            GraphicsDevice();
            ~GraphicsDevice();
            VertexBufferHandle createVertexBuffer(mem::Block* _mem, const VertexDecl& _decl);
		};
	}
}