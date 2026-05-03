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

    const auto& src = animations[index].payload().get();
    unsigned int bi = 0;
    for (auto& bone : bones) { bone.bone->animations.emplace_back(&src.getBoneAnimations()[bi++]); }
}

void Skeleton::playAnimation(const std::string& name, float weight) {
    for (unsigned int i = 0; i < animations.size(); ++i) {
        if (animations[i].payload().get().getName() == name) {
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
    for (unsigned int i = 0; i < animations.size(); ++i) {
        if (animations[i].payload().get().getName() == name) {
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
