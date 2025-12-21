#include <BLIB/Systems/SkeletalAnimationSystem.hpp>

#include <BLIB/Components/Bone.hpp>
#include <BLIB/Components/Transform3D.hpp>
#include <BLIB/Engine/Engine.hpp>
#include <glm/gtx/transform.hpp>

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
                        glm::quat rot         = anim.interpolateRotation(animState.time);
                        const glm::vec3 scale = anim.interpolateScale(animState.time);

                        if (i == 0) {
                            animatedRotation = rot;
                            animatedScale    = scale * animState.weight;
                            animatedPosition = pos * animState.weight;
                        }
                        else {
                            if (glm::dot(animatedRotation, rot) < 0.f) { rot = -rot; }
                            animatedRotation =
                                glm::slerp(animatedRotation,
                                           rot,
                                           animState.weight / (totalWeight + animState.weight));
                            animatedScale += scale * animState.weight;
                            animatedPosition += pos * animState.weight;
                        }

                        totalWeight += animState.weight;
                    }

                    bone.transform->setPosition(animatedPosition);
                    bone.transform->setRotation(animatedRotation);
                    bone.transform->setScale(animatedScale);
                }
                else { bone.transform->setTransform(bone.bone->nodeBindPoseLocal); }
            }
            skeleton.needsRefresh = false;
            skeleton.resourceLink.markForTransfer(&skeleton);
        }
    });
}

} // namespace sys
} // namespace bl
