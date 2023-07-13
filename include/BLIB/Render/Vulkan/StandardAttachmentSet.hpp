#ifndef BLIB_RENDER_VULKAN_STANDARDATTACHMENTSET_HPP
#define BLIB_RENDER_VULKAN_STANDARDATTACHMENTSET_HPP

#include <BLIB/Render/Vulkan/AttachmentSet.hpp>
#include <array>

namespace bl
{
namespace rc
{
namespace vk
{
class Swapchain;
class StandardAttachmentBuffers;

/**
 * @brief Attachment set containing the standard attachments for render frames in the engine
 *
 * @ingroup Renderer
 */
class StandardAttachmentSet : public AttachmentSet {
public:
    /**
     * @brief Creates an uninitialized attachment set
     */
    StandardAttachmentSet();

    /**
     * @brief Returns the handle to the image for the color attachment
     */
    constexpr VkImage colorImage() const;

    /**
     * @brief Returns the color attachment image view
     */
    constexpr VkImageView colorImageView() const;

    /**
     * @brief Returns the handle to the image for the depth attachment
     */
    constexpr VkImage depthImage() const;

    /**
     * @brief Returns the depth attachment image view
     */
    constexpr VkImageView depthImageView() const;

    /**
     * @brief Sets the attachments of this attachment set
     *
     * @param colorImage The image of the color attachment
     * @param colorImageView The image view of the color attachment
     * @param depthImage The image of the depth attachment
     * @param depthImageview The image view of the depth attachment
     */
    void setAttachments(VkImage colorImage, VkImageView colorImageView, VkImage depthImage,
                        VkImageView depthImageview);

protected:
    std::array<VkImage, 2> imageHandles;
    std::array<VkImageView, 2> imageViewHandles;

    static constexpr unsigned int ColorIndex = 0;
    static constexpr unsigned int DepthIndex = 1;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline StandardAttachmentSet::StandardAttachmentSet()
: AttachmentSet(imageHandles, imageViewHandles) {}

inline constexpr VkImage StandardAttachmentSet::colorImage() const {
    return imageHandles[ColorIndex];
}

inline constexpr VkImageView StandardAttachmentSet::colorImageView() const {
    return imageViewHandles[ColorIndex];
}

inline constexpr VkImage StandardAttachmentSet::depthImage() const {
    return imageHandles[DepthIndex];
}

inline constexpr VkImageView StandardAttachmentSet::depthImageView() const {
    return imageViewHandles[DepthIndex];
}

inline void StandardAttachmentSet::setAttachments(VkImage ci, VkImageView civ, VkImage di,
                                                  VkImageView div) {
    imageHandles[ColorIndex]     = ci;
    imageHandles[DepthIndex]     = di;
    imageViewHandles[ColorIndex] = civ;
    imageViewHandles[DepthIndex] = div;
}

} // namespace vk
} // namespace rc
} // namespace bl

#endif
