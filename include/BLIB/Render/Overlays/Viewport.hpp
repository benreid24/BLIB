#ifndef BLIB_RENDER_OVERLAYS_VIEWPORT_HPP
#define BLIB_RENDER_OVERLAYS_VIEWPORT_HPP

#include <SFML/Graphics/Rect.hpp>
#include <glad/vulkan.h>

namespace bl
{
namespace render
{
namespace ovy
{
/**
 * @brief Represents a relative or absolute viewport for overlays. Is defined with respect to the
 *        global or parent viewport
 *
 * @ingroup Renderer
 */
class Viewport {
public:
    /**
     * @brief Create a viewport that is relative to the global rendering region
     *
     * @param viewport The viewport in normalized coordinates [0, 1]
     * @return The newly created viewport
     */
    static Viewport absolute(const sf::FloatRect& viewport);

    /**
     * @brief Create a viewport that is relative to the parent rendering region
     *
     * @param viewport The viewport in normalized coordinates [0, 1]
     * @return The newly created viewport
     */
    static Viewport relative(const sf::FloatRect& viewport);

    /**
     * @brief Constructs the Vulkan viewport from this viewport
     *
     * @param global The global rendering region
     * @param parent The current rendering region
     * @return The new viewport to apply
     */
    VkViewport createViewport(const VkViewport& global, const VkViewport& parent) const;

    /**
     * @brief Converts the given viewport into a scissor of the same region
     *
     * @param viewport The viewport to convert to a scissor
     * @return The scissor to use to constrain to the given viewport
     */
    static VkRect2D viewportToScissor(const VkViewport& viewport);

    /**
     * @brief Converts the given scissor to a viewport of the same region
     *
     * @param scissor The scissor to convert to a viewport
     * @return A viewport of the same region as the scissor
     */
    static VkViewport scissorToViewport(const VkRect2D& scissor);

private:
    bool isAbsolute;
    sf::FloatRect region;

    Viewport(bool abs, const sf::FloatRect& region);
};

} // namespace ovy
} // namespace render
} // namespace bl

#endif
