#include "moti/moti.h"
#include "util.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "moti/renderer/graphics_device.h"
#include "moti/memory/block.h"
#include "moti/memory/memory.h"


void processMesh(aiMesh* mesh, const aiScene* scene, const moti::graphics::VertexDecl& decl, MeshGroup& group, moti::graphics::GraphicsDevice* device) {
    std::vector<VertexNormalTexCoords> vertices;
    std::vector<uint16_t> indices;

    vertices.reserve(mesh->mNumVertices);
    indices.reserve(mesh->mNumVertices);

    for (uint32_t i = 0; i < mesh->mNumVertices; ++i) {

        auto& pos = mesh->mVertices[i];
        auto& normal = mesh->mNormals[i];

        if (mesh->mTextureCoords[0]) {
            auto& uvs = mesh->mTextureCoords[0][i];
            vertices.emplace_back(VertexNormalTexCoords{ pos.x, pos.y, pos.z, normal.x, normal.y, normal.z, uvs.x, uvs.y });
        }
        else {
            vertices.emplace_back(VertexNormalTexCoords{ pos.x, pos.y, pos.z, normal.x, normal.y, normal.z, 0.f, 0.f });
        }

    }
    indices.reserve(3 * mesh->mNumFaces);
    for (uint32_t findex = 0; findex < mesh->mNumFaces; ++findex) {
        aiFace& face = mesh->mFaces[findex];
        for (uint32_t index = 0; index < face.mNumIndices; ++index) {
            indices.emplace_back(face.mIndices[index]);
        }
    }

    moti::memory::Block memory = moti::memory_globals::defaultAllocator().allocate(sizeof(VertexNormalTexCoords) * vertices.size());
    memcpy(memory.m_ptr, vertices.data(), sizeof(VertexNormalTexCoords) * vertices.size());
    group.m_vbo = device->createVertexBuffer(&memory, decl);
    moti::memory_globals::defaultAllocator().deallocate(memory);
    memory = moti::memory_globals::defaultAllocator().allocate(sizeof(uint16_t) * indices.size());
    memcpy(memory.m_ptr, indices.data(), sizeof(uint16_t) * indices.size());
    group.m_ibo = device->createIndexBuffer(&memory);
    moti::memory_globals::defaultAllocator().deallocate(memory);
    group.m_indices = indices.size();
}

void processNode(aiNode* node, const aiScene* scene, const moti::graphics::VertexDecl& decl, std::vector<MeshGroup>& groups, moti::graphics::GraphicsDevice* dev) {
    for (uint32_t i = 0; i < node->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        groups.emplace_back(MeshGroup{ 0 });
        processMesh(mesh, scene, decl, groups.back(), dev);
    }

    for (uint32_t i = 0; i < node->mNumChildren; ++i) {
        processNode(node->mChildren[i], scene, decl, groups, dev);
    }
}



void Mesh::load(const char* _path, moti::graphics::GraphicsDevice* device) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(_path, aiProcess_FlipUVs | aiProcess_Triangulate);

    if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        printf("Assimp error %s\n", importer.GetErrorString());
        return;
    }
    moti::graphics::VertexDecl decl;
    decl.begin().
        add(moti::Attribute::Position, 3, moti::AttributeType::Float, true).
        add(moti::Attribute::Normal, 3, moti::AttributeType::Float, true).
        add(moti::Attribute::TexCoord0, 2, moti::AttributeType::Float, true);

    processNode(scene->mRootNode, scene, decl, m_groups, device);
}

void Mesh::submit(moti::graphics::GraphicsDevice& device, moti::graphics::ProgramHandle program, const moti::Mat4& transform) const
{
    for (auto& group : m_groups) {
        device.setTransform(transform);
        device.setVertexBuffer(group.m_vbo, 0, group.m_indices);
        //device.setIndexBuffer(group.m_ibo, 0, group.m_indices);
        device.submit(program);
    }
}