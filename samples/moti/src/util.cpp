#include "moti/moti.h"
#include "util.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "moti/renderer/graphics_device.h"
#include "moti/memory/block.h"
#include "moti/memory/memory.h"
#include "moti/memory/stack_allocator.h"
#include "moti/io/io.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

void processMesh(aiMesh* mesh, const aiScene* scene, const moti::VertexDecl& decl, MeshGroup& group) {
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

    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        aiColor4D diffuse;
        aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuse);
        
        printf("diffuse count ");
    }

    indices.reserve(3 * mesh->mNumFaces);
    for (uint32_t findex = 0; findex < mesh->mNumFaces; ++findex) {
        aiFace& face = mesh->mFaces[findex];
        for (uint32_t index = 0; index < face.mNumIndices; ++index) {
            indices.emplace_back(face.mIndices[index]);
        }
    }

    moti::Block memory = moti::memory_globals::defaultAllocator().allocate(sizeof(VertexNormalTexCoords) * vertices.size());
    memcpy(memory.m_ptr, vertices.data(), sizeof(VertexNormalTexCoords) * vertices.size());
    group.m_vbo = moti::createVertexBuffer(&memory, decl);
    moti::memory_globals::defaultAllocator().deallocate(memory);
    memory = moti::memory_globals::defaultAllocator().allocate(sizeof(uint16_t) * indices.size());
    memcpy(memory.m_ptr, indices.data(), sizeof(uint16_t) * indices.size());
    group.m_ibo = moti::createIndexBuffer(&memory);
    moti::memory_globals::defaultAllocator().deallocate(memory);
    group.m_indices = indices.size();
}

void processNode(aiNode* node, const aiScene* scene, const moti::VertexDecl& decl, std::vector<MeshGroup>& groups) {
    for (uint32_t i = 0; i < node->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        groups.emplace_back(MeshGroup{ 0 });
        processMesh(mesh, scene, decl, groups.back());
    }

    for (uint32_t i = 0; i < node->mNumChildren; ++i) {
        processNode(node->mChildren[i], scene, decl, groups);
    }
}

moti::ShaderHandle create_shader(const char* src, uint32_t magic) {
    FILE* file = nullptr;
    fopen_s(&file, src, "rb");

    if (file == nullptr) {
        MOTI_TRACE("Failed to open file %s", src);
        __debugbreak();
    }

    fseek(file, 0, SEEK_END);
    uint32_t size = ftell(file);
    fseek(file, 0, SEEK_SET);

    const uint32_t MaxSize = 4096;
    if (size >= MaxSize) {
        fprintf(stderr, "Out of mem");
        __debugbreak();
    }
    moti::StackAllocator<MaxSize> alloc;
    moti::DynamicMemoryBlock memory(&alloc);
    moti::MemoryWriter writer(&memory);
    moti::write<uint32_t>(&writer, magic);
    moti::write<uint32_t>(&writer, size);
    fread((char*)(memory.m_data.m_ptr) + 2 * sizeof(uint32_t), sizeof(char), size, file);
    fclose(file);
    return moti::createShader(&memory.m_data);
}



moti::ProgramHandle load_program(const char* vshpath, const char* fshpath)
{

    moti::ShaderHandle vsh = create_shader(vshpath, MOTI_VERTEX_SHADER_MAGIC);
    moti::ShaderHandle fsh = create_shader(fshpath, MOTI_FRAGMENT_SHADER_MAGIC);
    moti::ProgramHandle program = moti::createProgram(vsh, fsh);
    moti::destroyShader(vsh);
    moti::destroyShader(fsh);
    
    return program;
}


moti::TextureHandle load_texture(const char* filePath)
{
    moti::TextureHandle handle{ UINT16_MAX };
    int width, height, n;
    unsigned char* data = stbi_load(filePath, &width, &height, &n, 0);

    if (data == NULL) {
        return handle;
    }

    uint32_t size = width * height;

    moti::Block block((void*)data, size);

    handle = moti::createTexture(width, height, &block);

    stbi_image_free(data);

    return handle;
}

void Mesh::load(const char* _path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(_path, aiProcess_FlipUVs | aiProcess_Triangulate);

    if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        printf("Assimp error %s\n", importer.GetErrorString());
        return;
    }
    moti::VertexDecl decl;
    decl.begin().
        add(moti::Attribute::Position, 3, moti::AttributeType::Float, true).
        add(moti::Attribute::Normal, 3, moti::AttributeType::Float, true).
        add(moti::Attribute::TexCoord0, 2, moti::AttributeType::Float, true);

    processNode(scene->mRootNode, scene, decl, m_groups);
}

void Mesh::submit(moti::ProgramHandle program, const moti::Mat4& transform) const
{
    for (auto& group : m_groups) {
        moti::setTransform(transform);
        moti::setVertexBuffer(group.m_vbo, 0, group.m_indices);
        moti::setIndexBuffer(group.m_ibo, 0, group.m_indices);
        moti::submit(program);
    }
}
