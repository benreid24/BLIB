#include <BLIB/Components/Skeleton.hpp>

#include <BLIB/Components/Bone.hpp>

namespace bl
{
namespace com
{
void Skeleton::playAnimation(unsigned int index, float weight) {
    auto& anim          = activeAnimations.emplace_back();
    anim.animationIndex = index;
    anim.weight         = weight;

    const auto& src = animations.getAnimations()[index];
    unsigned int bi = 0;
    for (auto& bone : bones) { bone.bone->animations.emplace_back(&src.getBoneAnimations()[bi++]); }
}

void Skeleton::playAnimation(const std::string& name, float weight) {
    const auto& anims = animations.getAnimations();
    for (unsigned int i = 0; i < anims.size(); ++i) {
        if (anims[i].getName() == name) {
            playAnimation(i, weight);
            return;
        }
    }
}

void Skeleton::stopAnimation(unsigned int index) {
    for (std::size_t i = 0; i < activeAnimations.size(); ++i) {
        if (activeAnimations[i].animationIndex == index) {
            activeAnimations.erase(i);
            needsRefresh = true;
            return;
        }
    }
}

void Skeleton::stopAnimation(const std::string& name) {
    const auto& anims = animations.getAnimations();
    for (unsigned int i = 0; i < anims.size(); ++i) {
        if (anims[i].getName() == name) {
            stopAnimation(i);
            return;
        }
    }
}

void Skeleton::stopAllAnimations() {
    activeAnimations.clear();
    needsRefresh = true;
}

} // namespace com
} // namespace bl
