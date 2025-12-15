#ifndef BLIB_COMPONENTS_SKELETONINDEXLINK_HPP
#define BLIB_COMPONENTS_SKELETONINDEXLINK_HPP

#include <BLIB/Render/Components/DescriptorComponentBase.hpp>

namespace bl
{
namespace com
{
/**
 * @brief Component to store the first bone index of a skeleton used by a skinned mesh
 *
 * @ingroup Components
 */
struct SkeletonIndexLink
: public rc::rcom::DescriptorComponentBase<SkeletonIndexLink, std::uint32_t> {
    std::uint32_t baseBoneIndex;

    /**
     * @brief Creates the component with default values
     */
    SkeletonIndexLink()
    : baseBoneIndex(0) {}

    /**
     * @brief Marks the component as needing an update
     */
    void markForUpdate() { markDirty(); }

    /**
     * @brief Updates the active transforms of all bones in the skeleton
     *
     * @param dst The output transform array
     */
    virtual void refreshDescriptor(std::uint32_t& offset) override { offset = baseBoneIndex; }
};

} // namespace com
} // namespace bl

#endif
