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
template<std::uint32_t N>
class AttachmentImageSet;

/**
 * @brief Base class representing an attachment set that can be rendered to. Derived classes can
 *        provide additional functionality
 *
 * @ingroup Renderer
 */
class AttachmentSet {
public:
    static constexpr std::uint32_t MaxAttachments = 8;

    /**
     * @brief Creates the attachment set from the images and views
     *
     * @tparam N The number of attachments
     * @param images The array of image handles
     * @param views The array of image view handles
     * @param aspects The aspect of each attachment
     * @param layerCount The number of layers in the underlying images
     * @param count The number of attachments in the set
     */
    template<std::size_t N>
    AttachmentSet(std::array<VkImage, N>& images, std::array<VkImageView, N>& views,
                  const std::array<VkImageAspectFlags, N>& aspects, std::uint32_t layerCount,
                  std::uint32_t count = N);

    /**
     * @brief Creates an attachment set for a single image
     *
     * @param image The image to init with
     */
    AttachmentSet(Image& image);

    /**
     * @brief Returns a pointer to the start of the images
     */
    const VkImage* images() const;

    /**
     * @brief Returns a pointer to the start of the image views
     */
    const VkImageView* imageViews() const;

    /**
     * @brief Returns a pointer to the start of the image aspects
     */
    const VkImageAspectFlags* imageAspects() const;

    /**
     * @brief Returns the number of attachments in the set
     */
    std::uint32_t size() const;

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
    const VkExtent2D& renderExtent() const;

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

protected:
    VkExtent2D extent;

private:
    const VkImage* imagesPointer;
    const VkImageView* viewsPointer;
    std::array<VkImageAspectFlags, MaxAttachments> aspects;
    std::uint32_t n;
    std::uint32_t layerCount;
    std::uint32_t outputIndex;

    AttachmentSet();

    template<std::size_t N>
    void init(std::array<VkImage, N>& images, std::array<VkImageView, N>& views,
              const std::array<VkImageAspectFlags, N>& aspects, std::uint32_t layerCount);

    template<std::uint32_t N>
    friend class AttachmentImageSet;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline const VkImage* AttachmentSet::images() const { return imagesPointer; }

inline const VkImageView* AttachmentSet::imageViews() const { return viewsPointer; }

inline const VkImageAspectFlags* AttachmentSet::imageAspects() const { return aspects.data(); }

inline std::uint32_t AttachmentSet::size() const { return n; }

inline void AttachmentSet::setAttachmentCount(std::uint32_t count) { n = count; }

inline const VkExtent2D& AttachmentSet::renderExtent() const { return extent; }

inline std::uint32_t AttachmentSet::getOutputIndex() const { return outputIndex; }

inline void AttachmentSet::setOutputIndex(std::uint32_t index) { outputIndex = index; }

inline AttachmentSet::AttachmentSet()
: extent{0, 0}
, imagesPointer(nullptr)
, viewsPointer(nullptr)
, n(0)
, layerCount(1)
, outputIndex(0) {}

inline AttachmentSet::AttachmentSet(Image& image)
: imagesPointer(image.getImagePointer())
, viewsPointer(image.getViewPointer())
, n(1)
, layerCount(image.getLayerCount())
, outputIndex(0) {
    aspects[0] = image.getAspect();
    extent     = image.getSize();
}

template<std::size_t N>
inline AttachmentSet::AttachmentSet(std::array<VkImage, N>& images,
                                    std::array<VkImageView, N>& views,
                                    const std::array<VkImageAspectFlags, N>& aspects,
                                    std::uint32_t layerCount, std::uint32_t count)
: imagesPointer(images.data())
, viewsPointer(views.data())
, n(count)
, layerCount(layerCount) {
    std::copy(aspects.begin(), aspects.end(), this->aspects.begin());
}

template<std::size_t N>
inline void AttachmentSet::init(std::array<VkImage, N>& images, std::array<VkImageView, N>& views,
                                const std::array<VkImageAspectFlags, N>& aspects,
                                std::uint32_t lc) {
    imagesPointer = images.data();
    viewsPointer  = views.data();
    n             = N;
    layerCount    = lc;
    std::copy(aspects.begin(), aspects.end(), this->aspects.begin());
}

inline void AttachmentSet::setRenderExtent(const VkExtent2D& e) { extent = e; }

inline std::uint32_t AttachmentSet::getLayerCount() const { return layerCount; }

} // namespace vk
} // namespace rc
} // namespace bl

#endif
