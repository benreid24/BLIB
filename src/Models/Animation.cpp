#include <BLIB/Models/Animation.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Models/BoneSet.hpp>

namespace bl
{
namespace mdl
{
Animation::Animation()
: durationInTicks(0.0)
, ticksPerSecond(0.0) {}

void Animation::populate(const aiAnimation* src, const NodeSet& nodes, const BoneSet& bones) {
    name            = src->mName.C_Str();
    durationInTicks = src->mDuration;
    ticksPerSecond  = src->mTicksPerSecond;

    boneAnimations.resize(src->mNumChannels);
    for (unsigned int i = 0; i < src->mNumChannels; ++i) {
        const auto& boneAnim = src->mChannels[i];
        const auto boneIndex = bones.getBoneIndexByName(
            std::string_view(boneAnim->mNodeName.data, boneAnim->mNodeName.length));
        if (!boneIndex.has_value()) {
            BL_LOG_WARN << "Animation '" << name << "' references unknown bone '"
                        << boneAnim->mNodeName.C_Str() << "'";
            continue;
        }
        boneAnimations[boneIndex.value()].populate(*boneAnim, nodes);
    }
}

} // namespace mdl
} // namespace bl
