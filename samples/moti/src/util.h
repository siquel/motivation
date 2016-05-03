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

struct MeshGroup {
    moti::graphics::VertexBufferHandle m_vbo;
    moti::graphics::IndexBufferHandle m_ibo;
    uint32_t m_indices;
};

struct Mesh {
    std::vector<MeshGroup> m_groups;
    void load(const char* _path, moti::graphics::GraphicsDevice* device);
    void submit(moti::graphics::GraphicsDevice&, moti::graphics::ProgramHandle handle, const moti::Mat4& transform) const;
};

moti::graphics::ProgramHandle load_program(const char* vshpath, const char* fshpath, moti::graphics::GraphicsDevice&);