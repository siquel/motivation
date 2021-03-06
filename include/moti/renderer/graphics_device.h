#pragma once

#include "moti/renderer/renderer_context.h"
#include "moti/math/math_types.h"

#include <unordered_map>
namespace moti {
	
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
            VertexBufferHandle createVertexBuffer(Block* _mem, const VertexDecl& _decl);
            IndexBufferHandle createIndexBuffer(Block* _mem);
            void setVertexBuffer(VertexBufferHandle _handle, uint32_t _startVertex, uint32_t _count);
            void setIndexBuffer(IndexBufferHandle _handle, uint32_t firstIndex, uint32_t _count);
            ShaderHandle createShader(Block* _mem);
            ProgramHandle createProgram(ShaderHandle _vertex, ShaderHandle _fragment);

            void destroyVertexBuffer(VertexBufferHandle _handle);
            void destroyIndexBuffer(IndexBufferHandle _handle);
            
            UniformHandle createUniform(UniformType::Enum _type, uint16_t _count, const char* _name);
            void destroyUniform(UniformHandle _handle);

            void destroyShader(ShaderHandle _handle);
            void destroyProgram(ProgramHandle _handle);
            void submit(ProgramHandle _program);

            void setViewTransform(const Mat4& _view, const Mat4& _proj);
            void setViewRect(uint32_t _x, uint32_t  _y, uint32_t _w, uint32_t h);
            void setTransform(const Mat4& _mtx);
            void setUniform(UniformHandle _handle, const void* _value);

            TextureHandle createTexture(Block* mem);
            void setTexture(uint8_t unit, UniformHandle sampler, TextureHandle texture);

            void updateTexture(TextureHandle handle, uint8_t side, const Rect& rect, Block& memroy);
		};
	
}