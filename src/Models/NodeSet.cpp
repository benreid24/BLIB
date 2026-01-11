#include <BLIB/Models/NodeSet.hpp>

namespace bl
{
namespace mdl
{
namespace
{
std::uint32_t countNodesRecursive(const aiNode* node) {
    std::uint32_t count = 1;
    for (std::uint32_t i = 0; i < node->mNumChildren; ++i) {
        count += countNodesRecursive(node->mChildren[i]);
    }
    return count;
}
} // namespace

void NodeSet::reserveSpace(const aiScene* scene) {
    nodes.reserve(countNodesRecursive(scene->mRootNode));
}

Node& NodeSet::addNode(std::uint32_t parent) { return nodes.emplace_back(parent, nodes.size()); }

void NodeSet::clearAllButRoot() {
    if (nodes.size() > 1) { nodes.erase(nodes.begin() + 1, nodes.end()); }
}

const Node* NodeSet::getNode(std::string_view name) const {
    for (const Node& node : nodes) {
        if (node.getName() == name) { return &node; }
    }
    return nullptr;
}

} // namespace mdl
} // namespace bl
