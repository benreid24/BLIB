#ifndef BLIB_RENDER_VULKAN_ATTACHMENTSET_HPP
#define BLIB_RENDER_VULKAN_ATTACHMENTSET_HPP

#include <array>
#include <glad/vulkan.h>

namespace bl
{
namespace render
{
/**
 * @brief Base class representing an attachment set that can be rendered to. Derived classes can
 *        provide additional functionality
 *
 * @ingroup Renderer
 */
class AttachmentSet {
public:
    /**
     * @brief Creates the attachment set from the images and views
     *
     * @tparam N The number of attachments
     * @param images The array of image handles
     * @param views The array of image view handles
     */
    template<std::size_t N>
    AttachmentSet(std::array<VkImage, N>& images, std::array<VkImageView, N>& views);

    /**
     * @brief Returns a pointer to the start of the images
     */
    constexpr const VkImage* images() const;

    /**
     * @brief Returns a pointer to the start of the image views
     */
    constexpr const VkImageView* imageViews() const;

    /**
     * @brief Returns the number of attachments in the set
     */
    constexpr std::uint32_t size() const;

    /**
     * @brief Returns the size of the renderable area
     *
     * @return The size of the renderable area
     */
    constexpr const VkExtent2D& renderExtent() const;

protected:
    VkExtent2D extent;

private:
    VkImage* imagesPointer;
    VkImageView* viewsPointer;
    std::uint32_t n;

    AttachmentSet() = delete;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr const VkImage* AttachmentSet::images() const { return imagesPointer; }

inline constexpr const VkImageView* AttachmentSet::imageViews() const { return viewsPointer; }

inline constexpr std::uint32_t AttachmentSet::size() const { return n; }

inline constexpr const VkExtent2D& AttachmentSet::renderExtent() const { return extent; }

template<std::size_t N>
inline AttachmentSet::AttachmentSet(std::array<VkImage, N>& images,
                                    std::array<VkImageView, N>& views)
: imagesPointer(images.data())
, viewsPointer(views.data())
, n(N) {}

} // namespace render
} // namespace bl

#endif
