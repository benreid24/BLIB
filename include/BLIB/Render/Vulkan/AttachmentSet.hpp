#ifndef BLIB_RENDER_VULKAN_ATTACHMENTSET_HPP
#define BLIB_RENDER_VULKAN_ATTACHMENTSET_HPP

#include <array>
#include <volk.h>

namespace bl
{
namespace gfx
{
namespace vk
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

inline void AttachmentSet::setRenderExtent(const VkExtent2D& e) { extent = e; }

} // namespace vk
} // namespace gfx
} // namespace bl

#endif
