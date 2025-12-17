#include <BLIB/Models/Animation.hpp>

namespace bl
{
namespace mdl
{
Animation::Animation()
: durationInTicks(0.0)
, ticksPerSecond(0.0) {}

void Animation::populate(const aiAnimation* src) {
    name            = src->mName.C_Str();
    durationInTicks = src->mDuration;
    ticksPerSecond  = src->mTicksPerSecond;

    boneAnimations.resize(src->mNumChannels);
    for (unsigned int i = 0; i < src->mNumChannels; ++i) {
        boneAnimations[i].populate(*src->mChannels[i]);
    }
}

} // namespace mdl
} // namespace bl
