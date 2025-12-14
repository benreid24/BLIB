#include <BLIB/Components/Skeleton.hpp>

#include <BLIB/Logging.hpp>

namespace bl
{
namespace com
{
Skeleton::Bone::Bone()
: parent(nullptr)
, boneOffset(1.f)
, nodeBindPoseTransform(1.f) {}

void Skeleton::Bone::init(const mdl::Model& model, const mdl::Node& node) {
    auto sourceIndex      = node.getBoneIndex();
    nodeBindPoseTransform = node.getTransform();

    if (sourceIndex.has_value()) {
        const auto& bone = model.getBones().getBone(sourceIndex.value());
        boneOffset       = bone.transform;
    }
    else { boneOffset = glm::mat4(1.f); }
}

void Skeleton::init(const mdl::Model& model) {
    bones.resize(model.getBones().numBones());

    if (!model.getRoot().getBoneIndex().has_value()) {
        BL_LOG_ERROR << "Model root node is not a bone, cannot create skeleton";
        return;
    }

    const auto initBone = [this, &model](const mdl::Node& node, Bone* parent, auto& self) -> Bone* {
        auto i = node.getBoneIndex();
        if (!i.has_value()) {
            i = bones.size();
            bones.emplace_back();
        }
        Bone* bone = &bones[i.value()];
        bone->init(model, node);
        bone->parent = parent;
        bone->children.reserve(node.getChildren().size());
        bone->localIndex = i.value();
        for (const auto& childNode : node.getChildren()) {
            bone->children.emplace_back(self(childNode, bone, self));
        }

        return bone;
    };

    root = &bones[model.getRoot().getBoneIndex().value()];
    initBone(model.getRoot(), nullptr, initBone);
}

void Skeleton::refreshDescriptor(std::uint32_t& offset) { offset = resourceLink.getOffset(); }

} // namespace com
} // namespace bl
