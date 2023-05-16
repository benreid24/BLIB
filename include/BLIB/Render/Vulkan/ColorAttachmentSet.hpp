#ifndef BLIB_RENDER_VULKAN_COLORATTACHMENTSET_HPP
#define BLIB_RENDER_VULKAN_COLORATTACHMENTSET_HPP

#include <BLIB/Render/Vulkan/AttachmentSet.hpp>
#include <array>

namespace bl
{
namespace render
{
namespace vk
{
class Swapchain;
class StandardImageBuffer;

/**
 * @brief Attachment set containing a single color attachment
 *
 * @ingroup Renderer
 */
class ColorAttachmentSet : public AttachmentSet {
public:
    /**
     * @brief Creates an uninitialized attachment set
     */
    ColorAttachmentSet();

    /**
     * @brief Returns the handle to the image for the color attachment
     */
    constexpr VkImage colorImage() const;

    /**
     * @brief Returns the color attachment image view
     */
    constexpr VkImageView colorImageView() const;

protected:
    std::array<VkImage, 1> imageHandles;
    std::array<VkImageView, 1> imageViewHandles;

private:
    friend class Swapchain;
    friend class StandardImageBuffer;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline ColorAttachmentSet::ColorAttachmentSet()
: AttachmentSet(imageHandles, imageViewHandles) {}

inline constexpr VkImage ColorAttachmentSet::colorImage() const { return imageHandles[0]; }

inline constexpr VkImageView ColorAttachmentSet::colorImageView() const {
    return imageViewHandles[0];
}

} // namespace vk
} // namespace render
} // namespace bl

#endif
