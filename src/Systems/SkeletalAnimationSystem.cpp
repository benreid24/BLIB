#include <BLIB/Systems/SkeletalAnimationSystem.hpp>

#include <BLIB/Components/Bone.hpp>
#include <BLIB/Components/Transform3D.hpp>
#include <BLIB/Engine/Engine.hpp>

namespace bl
{
namespace sys
{
void SkeletalAnimationSystem::init(engine::Engine& engine) {
    skeletons = &engine.ecs().getAllComponents<com::Skeleton>();
}

void SkeletalAnimationSystem::update(std::mutex&, float dt, float, float, float) {
    skeletons->forEach([dt](ecs::Entity, com::Skeleton& skeleton) {
        if (skeleton.needsRefresh || !skeleton.activeAnimations.empty()) {
            for (auto& bone : skeleton.bones) {
                glm::mat4 nodeTransform = bone.bone->nodeBindPoseLocal;

                // TODO - apply animation and replace nodeTransform if animation present

                bone.transform->setTransform(nodeTransform);
            }
            skeleton.needsRefresh = false;
            skeleton.resourceLink.markForTransfer(&skeleton);
        }
    });
}

} // namespace sys
} // namespace bl
