#include "moti/moti.h"
#include "util.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "moti/renderer/graphics_device.h"
#include "moti/memory/block.h"
#include "moti/memory/memory.h"


void Mesh::load(const char* _path, moti::graphics::GraphicsDevice* device) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(_path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        printf("Assimp error %s\n", importer.GetErrorString());
        return;
    }

    std::vector<VertexNormalTexCoords> vertices;
    std::vector<uint16_t> indices;
    
    aiNode* root = scene->mRootNode;

    for (uint32_t i = 0; i < root->mNumChildren; ++i) {
        aiNode* node = root->mChildren[i];
        for (uint32_t j = 0; j < node->mNumMeshes; ++j) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[j]];
            vertices.reserve(mesh->mNumVertices);
            for (uint32_t vindex = 0; vindex < mesh->mNumVertices; ++vindex) {
                auto& pos = mesh->mVertices[vindex];
                auto& normal = mesh->mNormals[vindex];

                if (mesh->mTextureCoords[0]) {
                    auto& uvs = mesh->mTextureCoords[0][vindex];
                    vertices.emplace_back(VertexNormalTexCoords{ pos.x, pos.y, pos.z, normal.x, normal.y, normal.z, uvs.x, uvs.y });
                }
                else {
                    vertices.emplace_back(VertexNormalTexCoords{ pos.x, pos.y, pos.z, normal.x, normal.y, normal.z, 0.f, 0.f });
                }
            }

            for (uint32_t findex = 0; findex < mesh->mNumFaces; ++findex) {
                aiFace& face = mesh->mFaces[findex];
                for (uint32_t index = 0; index < face.mNumIndices; ++index) {
                    indices.emplace_back(face.mIndices[index]);
                }
            }
        }
    }

    moti::graphics::VertexDecl decl;
    decl.begin().
        add(moti::Attribute::Position, 3, moti::AttributeType::Float, true).
        add(moti::Attribute::Normal, 3, moti::AttributeType::Float, true).
        add(moti::Attribute::TexCoord0, 2, moti::AttributeType::Float, true);
    
    moti::memory::Block memory = moti::memory_globals::defaultAllocator().allocate(sizeof(VertexNormalTexCoords) * vertices.size());
    memcpy(memory.m_ptr, vertices.data(), sizeof(VertexNormalTexCoords) * vertices.size());
    m_vbo = device->createVertexBuffer(&memory, decl);
    moti::memory_globals::defaultAllocator().deallocate(memory);
    memory = moti::memory_globals::defaultAllocator().allocate(sizeof(uint16_t) * indices.size());
    memcpy(memory.m_ptr, indices.data(), sizeof(uint16_t) * indices.size());
    m_ibo = device->createIndexBuffer(&memory);
    m_indices = indices.size();
}

void Mesh::submit(moti::graphics::GraphicsDevice& device, moti::graphics::ProgramHandle program, const moti::Mat4& transform) const
{
    device.setTransform(transform);
    device.setIndexBuffer(m_ibo, 0, m_indices);
    device.setVertexBuffer(m_vbo, 0, UINT32_MAX);
    device.submit(program);
}
