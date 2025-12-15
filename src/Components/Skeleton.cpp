#include <BLIB/Components/Skeleton.hpp>

#include <BLIB/Logging.hpp>

namespace bl
{
namespace com
{
Skeleton::Node::Node()
: parent(nullptr)
, boneOffset(1.f)
, nodeLocalTransform(1.f) {}

void Skeleton::Node::init(const mdl::Model& model, const mdl::Node& node) {
    boneIndex          = node.getBoneIndex();
    nodeLocalTransform = node.getTransform();

    if (boneIndex.has_value()) {
        const auto& bone = model.getBones().getBone(boneIndex.value());
        boneOffset       = bone.transform;
    }
    else { boneOffset = glm::mat4(1.f); }
}

void Skeleton::init(const mdl::Model& model) {
    numBones = model.getBones().numBones();
    nodes.resize(model.getNodes().size());

    const auto initNode = [this, &model](const mdl::Node& src, Node* parent, auto& self) -> Node* {
        auto i     = src.getOwnIndex();
        Node* node = &nodes[i];
        node->init(model, src);
        node->parent = parent;
        node->children.reserve(src.getChildren().size());
        for (const auto& ci : src.getChildren()) {
            const mdl::Node& childNode = model.getNodes().getNode(ci);
            node->children.emplace_back(self(childNode, node, self));
        }

        return node;
    };

    root = initNode(model.getRoot(), nullptr, initNode);
}

} // namespace com
} // namespace bl
