#include <BLIB/Models/AnimationSet.hpp>

namespace bl
{
namespace mdl
{
void AnimationSet::populate(const aiScene* scene, const NodeSet& nodes, const BoneSet& bones) {
    animations.resize(scene->mNumAnimations);
    for (unsigned int i = 0; i < scene->mNumAnimations; ++i) {
        animations[i].populate(scene->mAnimations[i], nodes, bones);
    }
}

} // namespace mdl
} // namespace bl
