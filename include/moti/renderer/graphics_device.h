#pragma once

#include "moti/renderer/renderer_context.h"
#include "moti/math/math_types.h"

#include <unordered_map>
namespace moti {
	namespace graphics {
		class GraphicsDevice {
        private:
            RendererContext* m_ctx;
            // TODO
            std::unordered_map<uint16_t, VertexDecl> m_declLookup;
            Render m_draw;
            Mat4 m_view;
            Mat4 m_proj;
            Rect m_viewRect;
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

            void setViewTransform(const Mat4& _view, const Mat4& _proj);
            void setViewRect(uint32_t _x, uint32_t  _y, uint32_t _w, uint32_t h);
		};
	}
}