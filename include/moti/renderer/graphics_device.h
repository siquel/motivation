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
            Render m_draw;
        public:
            GraphicsDevice();
            ~GraphicsDevice();
            VertexBufferHandle createVertexBuffer(mem::Block* _mem, const VertexDecl& _decl);
            IndexBufferHandle createIndexBuffer(mem::Block* _mem);
            void setVertexBuffer(VertexBufferHandle _handle, uint32_t _startVertex, uint32_t _count);
            void setIndexBuffer(IndexBufferHandle _handle, uint32_t firstIndex, uint32_t _count);
            ShaderHandle createShader(mem::Block* _mem);
            ProgramHandle createProgram(ShaderHandle _vertex, ShaderHandle _fragment);

            void destroyShader(ShaderHandle _handle);
            void destroyProgram(ProgramHandle _handle);
            void submit(ProgramHandle _program);
		};
	}
}