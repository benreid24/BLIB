#ifndef BLIB_COMPONENTS_SKELETON_HPP
#define BLIB_COMPONENTS_SKELETON_HPP

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

    std::vector<BoneLink> bones;
    rc::sri::SkeletalBonesResource::ComponentLink resourceLink;
    bool needsRefresh;

    /**
     * @brief Initializes with sane defaults
     */
    Skeleton() = default;
};

} // namespace com
} // namespace bl

#endif
