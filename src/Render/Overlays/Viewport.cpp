#include <BLIB/Render/Overlays/Viewport.hpp>

#include <cmath>

namespace bl
{
namespace gfx
{
namespace ovy
{
Viewport::Viewport(bool abs, const sf::FloatRect& v)
: isAbsolute(abs)
, region(v) {}

Viewport Viewport::absolute(const sf::FloatRect& v) { return Viewport{true, v}; }

Viewport Viewport::relative(const sf::FloatRect& v) { return Viewport{false, v}; }

VkViewport Viewport::createViewport(const VkViewport& global, const VkViewport& parent) const {
    const VkViewport& vp = isAbsolute ? global : parent;

    VkViewport result{};
    result.minDepth = vp.minDepth;
    result.maxDepth = vp.maxDepth;
    result.x        = vp.x + vp.width * region.left;
    result.y        = vp.y + vp.height * region.top;
    result.width    = vp.width * region.width;
    result.height   = vp.height * region.height;
    return result;
}

VkRect2D Viewport::viewportToScissor(const VkViewport& viewport) {
    VkRect2D result{};
    result.offset.x      = std::floor(viewport.x);
    result.offset.y      = std::floor(viewport.y);
    result.extent.width  = std::ceil(viewport.width);
    result.extent.height = std::ceil(viewport.height);
    return result;
}

VkViewport Viewport::scissorToViewport(const VkRect2D& scissor) {
    VkViewport result{};
    result.minDepth = 0.f;
    result.maxDepth = 1.f;
    result.x        = static_cast<float>(scissor.offset.x);
    result.y        = static_cast<float>(scissor.offset.y);
    result.width    = static_cast<float>(scissor.extent.width);
    result.height   = static_cast<float>(scissor.extent.height);
    return result;
}

} // namespace ovy
} // namespace gfx
} // namespace bl
