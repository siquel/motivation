#pragma once

#include "moti/renderer/renderer_context.h"
#include <unordered_map>
namespace moti {
	namespace graphics {
		class GraphicsDevice {
        private:
            RendererContext* m_ctx;
            // TODO
            std::unordered_map<uint16_t, VertexDecl> m_declLookup;
        public:
            GraphicsDevice();
            ~GraphicsDevice();
            VertexBufferHandle createVertexBuffer(mem::Block* _mem, const VertexDecl& _decl);
            void setVertexBuffer(VertexBufferHandle _handle);
            ShaderHandle createShader(mem::Block* _mem);
            ProgramHandle createProgram(ShaderHandle _vertex, ShaderHandle _fragment);

            void destroyShader(ShaderHandle _handle);
            void destroyProgram(ProgramHandle _handle);
		};
	}
}