#ifndef BLIB_COMPONENTS_SLIDESHOW_HPP
#define BLIB_COMPONENTS_SLIDESHOW_HPP

#include <BLIB/Components/Animation2DPlayer.hpp>
#include <BLIB/Render/Buffers/IndexBuffer.hpp>
#include <BLIB/Render/Components/DrawableBase.hpp>
#include <BLIB/Render/Primitives/SlideshowVertex.hpp>

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
    rc::buf::IndexBufferT<rc::prim::SlideshowVertex> indexBuffer;
    glm::vec2 size;

    /**
     * @brief Creates the index buffer to render the slideshow
     *
     * @param vulkanState Renderer Vulkan state
     * @param anim The animation to use when creating the vertices
     * @return True on success, false if animation is invalid slideshow
     */
    bool create(rc::vk::VulkanState& vulkanState, const Animation2DPlayer& anim);

    /**
     * @brief Changes the animation to a new player
     *
     * @param anim The new player to use
     * @return True on success, false if animation is invalid slideshow
     */
    bool setPlayer(const Animation2DPlayer& anim);

    /**
     * @brief Returns the default pipeline for regular scenes
     */
    virtual std::uint32_t getDefaultScenePipelineId() const override {
        return rc::Config::PipelineIds::SlideshowLit;
    }

    /**
     * @brief Returns the default pipeline for overlays
     */
    virtual std::uint32_t getDefaultOverlayPipelineId() const override {
        return rc::Config::PipelineIds::SlideshowUnlit;
    }
};

} // namespace com
} // namespace bl

#endif
