#ifndef BLIB_RENDER_VULKAN_ATTACHMENTSET_HPP
#define BLIB_RENDER_VULKAN_ATTACHMENTSET_HPP

#include <BLIB/Render/Vulkan/Image.hpp>
#include <BLIB/Vulkan.hpp>
#include <algorithm>
#include <array>
#include <cstdint>
#include <initializer_list>

namespace bl
{
namespace rc
{
namespace vk
{
class AttachmentImageSet;

/**
 * @brief Basic class containing a set of attachments for rendering
 *
 * @ingroup Renderer
 */
class AttachmentSet {
public:
    static constexpr std::uint32_t MaxAttachments = 16;

    /**
     * @brief Creates an empty attachment set
     */
    AttachmentSet();

    /**
     * @brief Creates the attachment set from the images and views
     *
     * @param attachmentCount The number of attachments in the set
     * @param images The array of image handles
     * @param views The array of image view handles
     * @param aspects The aspect of each attachment
     * @param extent The size of the renderable area
     * @param firstIndex The index of the first attachment in the set
     * @param layerCount The number of layers in the underlying images
     */
    AttachmentSet(std::uint32_t attachmentCount, VkImage* images, VkImageView* views,
                  VkImageAspectFlags* aspects, VkExtent2D extent, std::uint32_t layerCount = 1);

    /**
     * @brief Creates an attachment set for a single image
     *
     * @param image The image to init with
     */
    AttachmentSet(Image& image);

    /**
     * @brief Updates the attachment set with the given images and views
     *
     * @param attachmentCount The number of attachments in the set
     * @param images The array of image handles
     * @param views The array of image view handles
     * @param aspects The aspect of each attachment
     * @param extent The size of the renderable area
     * @param layerCount The number of layers in the underlying images
     */
    void init(std::uint32_t attachmentCount, VkImage* images, VkImageView* views,
              VkImageAspectFlags* aspects, VkExtent2D extent, std::uint32_t layerCount = 1);

    /**
     * @brief Creates an attachment set for a single image
     *
     * @param image The image to init with
     */
    void init(Image& image);

    /**
     * @brief Initializes the attachment set with the given images
     *
     * @param images The attachment image set to initialize with
     */
    void init(AttachmentImageSet& images);

    /**
     * @brief Sets a single attachment
     *
     * @param index The index of the attachment to set
     * @param image The image of the attachment
     * @param view The image view of the attachment
     */
    void setAttachment(std::uint32_t index, VkImage image, VkImageView view);

    /**
     * @brief Set the aspect of the attachment at the given index
     *
     * @param index The index of the attachment to set the aspect of
     * @param aspect The aspect to set for the attachment
     */
    void setAttachmentAspect(std::uint32_t index, VkImageAspectFlags aspect);

    /**
     * @brief Sets the attachment at the given index to the given image
     *
     * @param index The index of the attachment to set
     * @param image The image to set the attachment to
     */
    void setAttachment(std::uint32_t index, Image& image);

    /**
     * @brief Sets the attachments of this attachment set
     *
     * @param index The index to start writing
     * @param images The images to set the attachments to
     */
    void setAttachments(std::uint32_t index, AttachmentImageSet& images);

    /**
     * @brief Copies the attachments from the given attachment set
     *
     * @param other The attachment set to copy from
     * @param count The number of attachments to copy
     * @param baseIndex The start index to copy into
     */
    void copy(const AttachmentSet& other, std::uint32_t count, std::uint32_t baseIndex = 0);

    /**
     * @brief Returns a pointer to the start of the images
     */
    const VkImage* getImages() const;

    /**
     * @brief Returns the handle to the image at the given index
     *
     * @param i The index of the image to get
     * @return The image at the given index
     */
    VkImage getImage(std::uint32_t i) const;

    /**
     * @brief Returns a pointer to the start of the image views
     */
    const VkImageView* getImageViews() const;

    /**
     * @brief Returns the handle to the image view at the given index
     *
     * @param i The index of the image view to get
     * @return The image view at the given index
     */
    VkImageView getImageView(std::uint32_t i) const;

    /**
     * @brief Returns a pointer to the start of the image aspects
     */
    const VkImageAspectFlags* getImageAspects() const;

    /**
     * @brief Returns the aspect of the attachment at the given index
     *
     * @param i The index of the attachment to get the aspect of
     * @return The aspect of the attachment at the given index
     */
    VkImageAspectFlags getImageAspect(std::uint32_t i) const;

    /**
     * @brief Returns the number of attachments in the set
     */
    std::uint32_t getAttachmentCount() const;

    /**
     * @brief Sets the number of attachments in the set
     *
     * @param count The new number of attachments in the set
     */
    void setAttachmentCount(std::uint32_t count);

    /**
     * @brief Returns the size of the renderable area
     *
     * @return The size of the renderable area
     */
    const VkExtent2D& getRenderExtent() const;

    /**
     * @brief Sets the render extent of this attachment set
     *
     * @param extent The extent of rendering for this attachment set
     */
    void setRenderExtent(const VkExtent2D& extent);

    /**
     * @brief Returns the number of layers in the underlying images
     */
    std::uint32_t getLayerCount() const;

    /**
     * @brief Returns the index of the first output attachment in the set
     */
    std::uint32_t getOutputIndex() const;

    /**
     * @brief Sets the index of the first output attachment in the set
     *
     * @param index The index of the first output attachment in the set
     */
    void setOutputIndex(std::uint32_t index);

private:
    VkExtent2D extent;
    std::array<VkImage, MaxAttachments> images;
    std::array<VkImageView, MaxAttachments> views;
    std::array<VkImageAspectFlags, MaxAttachments> aspects;
    std::uint32_t n;
    std::uint32_t layerCount;
    std::uint32_t outputIndex;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline const VkImage* AttachmentSet::getImages() const { return images.data(); }

inline VkImage AttachmentSet::getImage(std::uint32_t i) const { return images[i]; }

inline const VkImageView* AttachmentSet::getImageViews() const { return views.data(); }

inline VkImageView AttachmentSet::getImageView(std::uint32_t i) const { return views[i]; }

inline const VkImageAspectFlags* AttachmentSet::getImageAspects() const { return aspects.data(); }

inline VkImageAspectFlags AttachmentSet::getImageAspect(std::uint32_t i) const {
    return aspects[i];
}

inline std::uint32_t AttachmentSet::getAttachmentCount() const { return n; }

inline const VkExtent2D& AttachmentSet::getRenderExtent() const { return extent; }

inline std::uint32_t AttachmentSet::getOutputIndex() const { return outputIndex; }

inline std::uint32_t AttachmentSet::getLayerCount() const { return layerCount; }

} // namespace vk
} // namespace rc
} // namespace bl

#endif
