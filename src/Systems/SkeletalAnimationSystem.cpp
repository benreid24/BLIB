#include <BLIB/Systems/SkeletalAnimationSystem.hpp>

#include <BLIB/Engine/Engine.hpp>

namespace bl
{
namespace sys
{
namespace
{
void processBone(com::Skeleton::Bone& bone, const glm::mat4& parentTransform, float dt) {
    glm::mat4 boneTransform = bone.nodeBindPoseTransform;

    // TODO - apply animation and replace boneTransform if animation present

    glm::mat4 globalTransform = parentTransform * boneTransform;

    // TODO - active may move. need global inverse?
    bone.activeTransform = globalTransform * bone.boneOffset;

    for (com::Skeleton::Bone* child : bone.children) { processBone(*child, globalTransform, dt); }
}
} // namespace

void SkeletalAnimationSystem::init(engine::Engine& engine) {
    skeletons = &engine.ecs().getAllComponents<com::Skeleton>();
}

void SkeletalAnimationSystem::update(std::mutex& stageMutex, float dt, float realDt, float residual,
                                     float realResidual) {
    skeletons->forEach([dt](ecs::Entity entity, com::Skeleton& skeleton) {
        // TODO - not all skeletons will need updating each frame
        glm::mat4 identity(1.f);
        processBone(*skeleton.root, identity, dt);
    });
}

} // namespace sys
} // namespace bl
