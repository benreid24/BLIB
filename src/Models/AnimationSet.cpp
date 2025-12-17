#include <BLIB/Models/AnimationSet.hpp>

namespace bl
{
namespace mdl
{
void AnimationSet::populate(const aiScene* scene) {
    animations.resize(scene->mNumAnimations);
    for (unsigned int i = 0; i < scene->mNumAnimations; ++i) {
        animations[i].populate(scene->mAnimations[i]);
    }
}

} // namespace mdl
} // namespace bl
