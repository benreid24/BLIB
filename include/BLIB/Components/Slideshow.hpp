#ifndef BLIB_COMPONENTS_SLIDESHOW_HPP
#define BLIB_COMPONENTS_SLIDESHOW_HPP

#include <BLIB/Graphics/Animation2D/AnimationData.hpp>
#include <BLIB/Render/Buffers/IndexBuffer.hpp>
#include <BLIB/Render/Components/DrawableBase.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
struct VulkanState;
}
} // namespace rc

namespace com
{
/**
 * @brief Drawable component for slideshow type 2d animations
 *
 * @ingroup Components
 */
struct Slideshow : public rc::rcom::DrawableBase {
    rc::buf::IndexBuffer indexBuffer;
    glm::vec2 size;

    /**
     * @brief Creates the index buffer to render the slideshow
     *
     * @param vulkanState Renderer Vulkan state
     * @param anim The animation to use when creating the vertices
     * @return True on success, false if animation is invalid slideshow
     */
    bool create(rc::vk::VulkanState& vulkanState, const gfx::a2d::AnimationData& anim);
};

} // namespace com
} // namespace bl

#endif
