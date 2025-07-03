#ifndef BLIB_RENDER_VULKAN_GENERICATTACHMENTSET_HPP
#define BLIB_RENDER_VULKAN_GENERICATTACHMENTSET_HPP

#include <BLIB/Render/Vulkan/AttachmentSet.hpp>
#include <array>
#include <cstdint>

namespace bl
{
namespace rc
{
namespace vk
{
/**
 * @brief Attachment set with storage for N attachment descriptors
 *
 * @ingroup Renderer
 */
template<std::uint32_t N>
class GenericAttachmentSet : public AttachmentSet {
public:
    /**
     * @brief Creates an uninitialized attachment set
     *
     * @param aspects The aspect of each attachment
     * @param layerCount The number of layers in the underlying images
     */
    GenericAttachmentSet(const std::array<VkImageAspectFlags, N>& aspects,
                         std::uint32_t layerCount);

    /**
     * @brief Returns the handle to the image at the given index
     *
     * @param i The index of the image to get
     * @return The image at the given index
     */
    VkImage getImage(std::uint32_t i) const;

    /**
     * @brief Returns the handle to the image view at the given index
     *
     * @param i The index of the image view to get
     * @return The image view at the given index
     */
    VkImageView getImageView(std::uint32_t i) const;

    /**
     * @brief Sets the attachments of this attachment set
     *
     * @param images The images of the attachment set
     * @param views The image views of the attachment set
     */
    void setAttachments(const std::array<VkImage, N>& images,
                        const std::array<VkImageView, N>& views);

    /**
     * @brief Sets a single attachment
     *
     * @param index The index of the attachment to set
     * @param image The image of the attachment
     * @param view The image view of the attachment
     */
    void setAttachment(std::uint32_t index, VkImage image, VkImageView view);

    /**
     * @brief Copies the attachments from the given attachment set
     *
     * @param other The attachment set to copy from
     * @param count The number of attachments to copy
     * @param baseIndex The start index to copy into
     */
    void copy(const AttachmentSet& other, std::uint32_t count = N, std::uint32_t baseIndex = 0);

protected:
    std::array<VkImage, N> imageHandles;
    std::array<VkImageView, N> imageViewHandles;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<std::uint32_t N>
GenericAttachmentSet<N>::GenericAttachmentSet(const std::array<VkImageAspectFlags, N>& aspects,
                                              std::uint32_t layerCount)
: AttachmentSet(imageHandles, imageViewHandles, aspects, layerCount) {}

template<std::uint32_t N>
VkImage GenericAttachmentSet<N>::getImage(std::uint32_t i) const {
    return imageHandles[i];
}

template<std::uint32_t N>
VkImageView GenericAttachmentSet<N>::getImageView(std::uint32_t i) const {
    return imageViewHandles[i];
}

template<std::uint32_t N>
void GenericAttachmentSet<N>::setAttachments(const std::array<VkImage, N>& images,
                                             const std::array<VkImageView, N>& views) {
    imageHandles     = images;
    imageViewHandles = views;
}

template<std::uint32_t N>
void GenericAttachmentSet<N>::setAttachment(std::uint32_t index, VkImage image, VkImageView view) {
    imageHandles[index]     = image;
    imageViewHandles[index] = view;
}

template<std::uint32_t N>
void GenericAttachmentSet<N>::copy(const AttachmentSet& other, std::uint32_t count,
                                   std::uint32_t baseIndex) {
    for (std::uint32_t i = 0; i < count; ++i) {
        imageHandles[baseIndex + i]     = other.images()[i];
        imageViewHandles[baseIndex + i] = other.imageViews()[i];
    }
}

} // namespace vk
} // namespace rc
} // namespace bl

#endif
