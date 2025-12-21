#ifndef BLIB_COMPONENTS_SKELETON_HPP
#define BLIB_COMPONENTS_SKELETON_HPP

#include <BLIB/Containers/StaticVector.hpp>
#include <BLIB/Models/Model.hpp>
#include <BLIB/Render/Components/DescriptorComponentBase.hpp>
#include <BLIB/Render/ShaderResources/SkeletalBonesResource.hpp>

namespace bl
{
namespace com
{
struct Bone;
class Transform3D;

/**
 * @brief A skeleton of bones for skeletal animation
 *
 * @ingroup Components
 */
class Skeleton {
public:
    struct BoneLink {
        Transform3D* transform;
        Bone* bone;

        BoneLink()
        : transform(nullptr)
        , bone(nullptr) {}
    };

    struct AnimationState {
        float time;
        float weight;
        std::uint32_t animationIndex;

        AnimationState()
        : time(0.f)
        , weight(1.f)
        , animationIndex(0) {}
    };

    std::vector<BoneLink> bones;
    rc::sri::SkeletalBonesResource::ComponentLink resourceLink;
    mdl::AnimationSet animations; // TODO - better interface. Separate component? external storage?
    ctr::StaticVector<AnimationState, 4> activeAnimations;
    bool needsRefresh;

    /**
     * @brief Initializes with sane defaults
     */
    Skeleton() = default;
};

} // namespace com
} // namespace bl

#endif
