#ifndef BLIB_RENDER_VULKAN_RENDERFRAME_HPP
#define BLIB_RENDER_VULKAN_RENDERFRAME_HPP

#include <glad/vulkan.h>
#include <vector>

namespace bl
{
namespace render
{
/**
 * @brief Base class for render target images
 * 
 * @ingroup Renderer
*/
class RenderFrame {
public:
    /**
     * @brief Returns the VkImage for the color attachment
     * 
     * @return The VkImage for the color attachment
    */
    constexpr VkImage colorImage() const;

    /**
     * @brief Returns the VkImageView for the color attachment
     * 
     * @return The VkImageView for the color attachment
    */
    constexpr VkImageView colorImageView() const;

    /**
     * @brief Returns the size of the renderable area
     * 
     * @return The size of the renderable area
    */
    constexpr const VkExtent2D& renderExtent() const;

protected:
    VkImage colorImageHandle;
    VkImageView colorImageViewHandle;
    VkExtent2D extent;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr VkImage RenderFrame::colorImage() const { return colorImageHandle; }

inline constexpr VkImageView RenderFrame::colorImageView() const { return colorImageViewHandle; }

inline constexpr const VkExtent2D& RenderFrame::renderExtent() const { return extent; }

} // namespace render
} // namespace bl

#endif
