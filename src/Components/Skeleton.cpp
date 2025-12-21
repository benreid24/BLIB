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

} // namespace com
} // namespace bl
