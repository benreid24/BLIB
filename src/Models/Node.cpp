#include <BLIB/Models/Node.hpp>

#include <BLIB/Models/ConversionHelpers.hpp>

namespace bl
{
namespace mdl
{
Node::Node()
: parent(nullptr) {}

void Node::populate(const aiScene* scene, const aiNode* src, BoneSet& bones) {
    transform = Convert::toMat4(src->mTransformation);

    meshes.reserve(src->mNumMeshes);
    for (unsigned int i = 0; i < src->mNumMeshes; ++i) {
        // TODO - with a buffer refactor we can share mesh buffers across entities
        meshes.emplace_back();
        meshes.back().populate(scene->mMeshes[src->mMeshes[i]], bones);
    }

    children.reserve(src->mNumChildren);
    for (unsigned int i = 0; i < src->mNumChildren; ++i) {
        children.emplace_back();
        children.back().populate(scene, src->mChildren[i], bones);
        children.back().parent = this;
    }
}

} // namespace mdl
} // namespace bl
