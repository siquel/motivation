#pragma once
#include <vector>
#include "moti/math/math_types.h"
#include "moti/renderer/renderer_context.h"

struct VertexNormalTexCoords {
    moti::Vec3 m_pos;
    moti::Vec3 m_normal;
    float u;
    float v;
};
MOTI_FORWARD_DECLARE(moti, graphics, GraphicsDevice);
struct Mesh {

    moti::graphics::VertexBufferHandle m_vbo;
    moti::graphics::IndexBufferHandle m_ibo;

    void load(const char* _path, moti::graphics::GraphicsDevice* device);
};