#ifndef BLIB_RENDER_VULKAN_ATTACHMENTSET_HPP
#define BLIB_RENDER_VULKAN_ATTACHMENTSET_HPP

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
class AttachmentBufferSet;

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
     */
    template<std::size_t N>
    AttachmentSet(std::array<VkImage, N>& images, std::array<VkImageView, N>& views,
                  const std::array<VkImageAspectFlags, N>& aspects);

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

protected:
    VkExtent2D extent;

private:
    VkImage* imagesPointer;
    VkImageView* viewsPointer;
    std::array<VkImageAspectFlags, MaxAttachments> aspects;
    std::uint32_t n;

    AttachmentSet();

    template<std::size_t N>
    void init(std::array<VkImage, N>& images, std::array<VkImageView, N>& views,
              const std::array<VkImageAspectFlags, N>& aspects);

    template<std::uint32_t N>
    friend class AttachmentBufferSet;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline const VkImage* AttachmentSet::images() const { return imagesPointer; }

inline const VkImageView* AttachmentSet::imageViews() const { return viewsPointer; }

inline const VkImageAspectFlags* AttachmentSet::imageAspects() const { return aspects.data(); }

inline std::uint32_t AttachmentSet::size() const { return n; }

inline const VkExtent2D& AttachmentSet::renderExtent() const { return extent; }

inline AttachmentSet::AttachmentSet()
: extent{0, 0}
, imagesPointer(nullptr)
, viewsPointer(nullptr)
, n(0) {}

template<std::size_t N>
inline AttachmentSet::AttachmentSet(std::array<VkImage, N>& images,
                                    std::array<VkImageView, N>& views,
                                    const std::array<VkImageAspectFlags, N>& aspects)
: imagesPointer(images.data())
, viewsPointer(views.data())
, n(N) {
    std::copy(aspects.begin(), aspects.end(), this->aspects.begin());
}

template<std::size_t N>
inline void AttachmentSet::init(std::array<VkImage, N>& images, std::array<VkImageView, N>& views,
                                const std::array<VkImageAspectFlags, N>& aspects) {
    imagesPointer = images.data();
    viewsPointer  = views.data();
    n             = N;
    std::copy(aspects.begin(), aspects.end(), this->aspects.begin());
}

inline void AttachmentSet::setRenderExtent(const VkExtent2D& e) { extent = e; }

} // namespace vk
} // namespace rc
} // namespace bl

#endif
