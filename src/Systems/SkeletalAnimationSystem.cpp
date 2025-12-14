#include <BLIB/Systems/SkeletalAnimationSystem.hpp>

#include <BLIB/Engine/Engine.hpp>

namespace bl
{
namespace sys
{
namespace
{
void processBone(rc::sri::SkeletalBonesResource::ComponentLink& link, com::Skeleton::Bone& bone,
                 const glm::mat4& parentTransform, float dt) {
    glm::mat4 boneTransform = bone.nodeBindPoseTransform;

    // TODO - apply animation and replace boneTransform if animation present

    glm::mat4 globalTransform = parentTransform * boneTransform;

    link.getBaseWritePtr()[bone.localIndex] = globalTransform * bone.boneOffset;

    for (com::Skeleton::Bone* child : bone.children) {
        processBone(link, *child, globalTransform, dt);
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
            processBone(skeleton.resourceLink, *skeleton.root, identity, dt);
            skeleton.needsRefresh = false;
            skeleton.resourceLink.markForTransfer();
        }
    });
}

} // namespace sys
} // namespace bl
