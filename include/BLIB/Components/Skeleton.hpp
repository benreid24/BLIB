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
    com::Transform3D* worldTransform;
    rc::sri::SkeletalBonesResource::ComponentLink resourceLink;
    mdl::AnimationSet animations; // TODO - better interface. Separate component? external storage?
    ctr::StaticVector<AnimationState, 4> activeAnimations;
    bool needsRefresh;

    /**
     * @brief Plays an animation on this skeleton
     *
     * @param index The index of the animation to play
     * @param weight The weight of the animation, used for blending multiple animations
     */
    void playAnimation(unsigned int index, float weight = 1.f);

    /**
     * @brief Plays the animation with the given name
     *
     * @param name The name of the animation to play
     * @param weight The blend weight of the animation
     */
    void playAnimation(const std::string& name, float weight = 1.f);

    /**
     * @brief Stops the animation at the given index
     *
     * @param index The index of the animation to stop
     */
    void stopAnimation(unsigned int index);

    /**
     * @brief Stops the animation with the given name
     *
     * @param name The name of the animation to stop
     */
    void stopAnimation(const std::string& name);

    /**
     * @brief Stops all animations
     */
    void stopAllAnimations();

    /**
     * @brief Initializes with sane defaults
     */
    Skeleton() = default;
};

} // namespace com
} // namespace bl

#endif
