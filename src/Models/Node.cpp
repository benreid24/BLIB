#include <BLIB/Models/Node.hpp>

#include <BLIB/Models/BoneSet.hpp>
#include <BLIB/Models/ConversionHelpers.hpp>
#include <BLIB/Models/MeshSet.hpp>
#include <BLIB/Models/NodeSet.hpp>

namespace bl
{
namespace mdl
{
Node::Node()
: parent(0)
, ownIndex(0) {}

Node::Node(std::uint32_t parentIndex, std::uint32_t ownIdx)
: parent(parentIndex)
, ownIndex(ownIdx) {}

void Node::populate(NodeSet& nodeSet, const aiScene* scene, const aiNode* src, BoneSet& bones) {
    name      = std::string(src->mName.C_Str());
    transform = Convert::toMat4(src->mTransformation);

    meshes.reserve(src->mNumMeshes);
    for (unsigned int i = 0; i < src->mNumMeshes; ++i) { meshes.emplace_back(src->mMeshes[i]); }

    children.reserve(src->mNumChildren);
    for (unsigned int i = 0; i < src->mNumChildren; ++i) {
        Node& childNode = nodeSet.addNode(ownIndex);
        children.emplace_back(childNode.getOwnIndex());
        childNode.populate(nodeSet, scene, src->mChildren[i], bones);
    }

    boneIndex = bones.getBoneIndexByName(name);
}

} // namespace mdl
} // namespace bl
