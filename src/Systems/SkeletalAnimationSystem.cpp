#include <BLIB/Systems/SkeletalAnimationSystem.hpp>

#include <BLIB/Components/Bone.hpp>
#include <BLIB/Components/Transform3D.hpp>
#include <BLIB/Engine/Engine.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
            for (auto& animation : skeleton.activeAnimations) {
                const auto& src = skeleton.animations.getAnimations()[animation.animationIndex];
                animation.time += dt * src.getTicksPerSecond();
                if (animation.time > src.getDurationInTicks()) {
                    animation.time =
                        std::fmodf(animation.time, static_cast<float>(src.getDurationInTicks()));
                }
            }

            for (auto& bone : skeleton.bones) {
                glm::mat4 nodeTransform = bone.bone->nodeBindPoseLocal;

                if (!bone.bone->animations.empty()) {
                    glm::vec3 animatedPosition(0.f);
                    glm::quat animatedRotation(1.f, 0.f, 0.f, 0.f);
                    glm::vec3 animatedScale(1.f);
                    float totalWeight = 0.f;

                    for (unsigned int i = 0; i < bone.bone->animations.size(); ++i) {
                        // TODO - indices may not always line up?
                        const auto& animState = skeleton.activeAnimations[i];
                        const auto& anim      = *bone.bone->animations[i];
                        const glm::vec3 pos   = anim.interpolatePosition(animState.time);
                        const glm::quat rot   = anim.interpolateRotation(animState.time);
                        const glm::vec3 scale = anim.interpolateScale(animState.time);

                        animatedPosition = animatedPosition * totalWeight + pos * animState.weight;
                        animatedRotation =
                            glm::slerp(animatedRotation,
                                       rot,
                                       animState.weight / (totalWeight + animState.weight));
                        animatedScale = animatedScale * totalWeight + scale * animState.weight;
                        totalWeight   = animState.weight;
                    }

                    nodeTransform = glm::translate(glm::mat4(1.f), animatedPosition) *
                                    glm::toMat4(animatedRotation) *
                                    glm::scale(glm::mat4(1.f), animatedScale);
                }

                bone.transform->setTransform(nodeTransform);
            }
            skeleton.needsRefresh = false;
            skeleton.resourceLink.markForTransfer(&skeleton);
        }
    });
}

} // namespace sys
} // namespace bl
