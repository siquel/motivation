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

struct MeshGroup {
    moti::VertexBufferHandle m_vbo;
    moti::IndexBufferHandle m_ibo;
    uint32_t m_indices;
};

struct Mesh {
    std::vector<MeshGroup> m_groups;
    void load(const char* _path);
    void submit(moti::ProgramHandle handle, const moti::Mat4& transform) const;
};

moti::ProgramHandle load_program(const char* vshpath, const char* fshpath);

moti::TextureHandle load_texture(const char* filePath);