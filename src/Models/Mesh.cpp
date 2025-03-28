#include <BLIB/Models/Mesh.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Models/BoneSet.hpp>
#include <BLIB/Models/ConversionHelpers.hpp>
#include <stdexcept>

namespace bl
{
namespace mdl
{
Mesh::Mesh()
: materialIndex(0) {}

Mesh::Mesh(const Mesh& src, const glm::mat4& transform)
: materialIndex(src.materialIndex)
, indices(src.indices) {
    transformVertices(src.vertices, transform);
}

void Mesh::combine(const Mesh& other, const glm::mat4& transform) {
    indices.reserve(indices.size() + other.indices.size());
    for (const std::uint32_t i : other.indices) { indices.emplace_back(i + vertices.size()); }
    transformVertices(other.vertices, transform);
}

void Mesh::transformVertices(const std::vector<Vertex>& src, const glm::mat4& transform) {
    const glm::mat4 normalTransform = glm::transpose(glm::inverse(transform));
    vertices.reserve(vertices.size() + src.size());
    for (const Vertex& v : src) {
        Vertex newV  = v;
        newV.pos     = transform * glm::vec4(v.pos, 1.f);
        newV.normal  = glm::normalize(normalTransform * glm::vec4(v.normal, 0.f));
        newV.tangent = glm::normalize(normalTransform * glm::vec4(v.tangent, 0.f));
        vertices.emplace_back(newV);
    }
}

void Mesh::populate(const aiMesh* src, BoneSet& bones) {
    if (src->mPrimitiveTypes != aiPrimitiveType::aiPrimitiveType_TRIANGLE) {
        throw std::runtime_error("Meshes must be triangles");
    }

    materialIndex = src->mMaterialIndex;

    vertices.resize(src->mNumVertices);
    indices.resize(src->mNumFaces * 3);

    for (unsigned int i = 0; i < src->mNumFaces; ++i) {
        const aiFace& face = src->mFaces[i];
        if (face.mNumIndices != 3) {
            BL_LOG_ERROR << "Found face with more than 3 indices";
            indices.resize(indices.size() - 3);
            continue;
        }
        for (unsigned int j = 0; j < face.mNumIndices; ++j) {
            indices[i * 3 + j] = face.mIndices[j];
        }
    }

    for (unsigned int i = 0; i < src->mNumVertices; ++i) {
        vertices[i].pos = Convert::toVec3(src->mVertices[i]);
        if (src->mNormals) { vertices[i].normal = Convert::toVec3(src->mNormals[i]); }
        if (src->mTangents) { vertices[i].tangent = Convert::toVec3(src->mTangents[i]); }
        if (src->mBitangents) { vertices[i].bitangent = Convert::toVec3(src->mBitangents[i]); }
        if (src->mColors[0]) { vertices[i].color = Convert::toVec4(src->mColors[0][i]); }
        if (src->mTextureCoords[0]) {
            vertices[i].texCoord = Convert::toVec2(src->mTextureCoords[0][i]);
        }
    }

    if (src->mNumBones > 0) {
        for (unsigned int j = 0; j < src->mNumBones; ++j) {
            const aiBone* bone = src->mBones[j];
            for (unsigned int k = 0; k < bone->mNumWeights; ++k) {
                const aiVertexWeight* weight = &bone->mWeights[k];
                Vertex& v                    = vertices[weight->mVertexId];
                for (unsigned int i = 0; i < 4; ++i) {
                    if (v.boneIds[i] == -1) {
                        v.boneIds[i]     = bones.getOrAddBone(bone);
                        v.boneWeights[i] = weight->mWeight;
                    }
                }
            }
        }
    }
}

} // namespace mdl
} // namespace bl
