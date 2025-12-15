#include <BLIB/Models/Node.hpp>

#include <BLIB/Models/BoneSet.hpp>
#include <BLIB/Models/ConversionHelpers.hpp>
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
    for (unsigned int i = 0; i < src->mNumMeshes; ++i) {
        // TODO - with a buffer refactor we can share mesh buffers across entities
        meshes.emplace_back();
        meshes.back().populate(scene->mMeshes[src->mMeshes[i]], bones);
    }

    children.reserve(src->mNumChildren);
    for (unsigned int i = 0; i < src->mNumChildren; ++i) {
        Node& childNode = nodeSet.addNode(ownIndex);
        children.emplace_back(childNode.getOwnIndex());
        childNode.populate(nodeSet, scene, src->mChildren[i], bones);
    }

    boneIndex = bones.getBoneIndexByName(name);
}

void Node::mergeChildren(NodeSet& nodes) {
    for (std::uint32_t ci : children) {
        Node& child = nodes.getNode(ci);
        child.mergeChildren(nodes);
        for (const Mesh& mesh : child.getMeshes()) {
            bool merged = false;
            for (auto& m : meshes) {
                if (m.getMaterialIndex() == mesh.getMaterialIndex()) {
                    m.combine(mesh, child.getTransform());
                    merged = true;
                    break;
                }
            }
            if (!merged) { meshes.emplace_back(mesh, child.getTransform()); }
        }
    }
    children.clear();
}

} // namespace mdl
} // namespace bl
