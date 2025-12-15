#include <BLIB/Systems/SkeletalAnimationSystem.hpp>

#include <BLIB/Engine/Engine.hpp>

namespace bl
{
namespace sys
{
namespace
{
void processNode(rc::sri::SkeletalBonesResource::ComponentLink& link, com::Skeleton::Node& node,
                 const glm::mat4& parentTransform, const glm::mat4& globalRootInverse, float dt) {
    glm::mat4 nodeTransform = node.nodeLocalTransform;

    // TODO - apply animation and replace nodeTransform if animation present

    glm::mat4 globalTransform = parentTransform * nodeTransform;

    if (link.linked() && node.boneIndex.has_value()) {
        link.getBaseWritePtr()[node.boneIndex.value()] =
            globalRootInverse * globalTransform * node.boneOffset;
    }

    for (com::Skeleton::Node* child : node.children) {
        processNode(link, *child, globalTransform, globalRootInverse, dt);
    }
}
} // namespace

void SkeletalAnimationSystem::init(engine::Engine& engine) {
    skeletons = &engine.ecs().getAllComponents<com::Skeleton>();
}

void SkeletalAnimationSystem::update(std::mutex&, float dt, float, float, float) {
    skeletons->forEach([dt](ecs::Entity, com::Skeleton& skeleton) {
        if (skeleton.needsRefresh) {
            glm::mat4 identity(1.f);
            glm::mat4 globalRootInverse = glm::inverse(skeleton.root->nodeLocalTransform);
            processNode(skeleton.resourceLink, *skeleton.root, identity, globalRootInverse, dt);
            skeleton.needsRefresh = false;
            skeleton.resourceLink.markForTransfer();
        }
    });
}

} // namespace sys
} // namespace bl
