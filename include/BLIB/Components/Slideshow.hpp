#ifndef BLIB_COMPONENTS_SLIDESHOW_HPP
#define BLIB_COMPONENTS_SLIDESHOW_HPP

#include <BLIB/Components/Animation2DPlayer.hpp>
#include <BLIB/Render/Buffers/IndexBuffer.hpp>
#include <BLIB/Render/Components/DrawableBase.hpp>
#include <BLIB/Render/Config/MaterialPipelineIds.hpp>
#include <BLIB/Render/Primitives/SlideshowVertex.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
struct VulkanLayer;
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
    bool create(rc::vk::VulkanLayer& vulkanState, const Animation2DPlayer& anim);

    /**
     * @brief Changes the animation to a new player
     *
     * @param anim The new player to use
     * @return True on success, false if animation is invalid slideshow
     */
    bool setPlayer(const Animation2DPlayer& anim);

    /**
     * @brief Returns the default material pipeline for rendering
     */
    virtual std::uint32_t getDefaultMaterialPipelineId() const override {
        return rc::cfg::MaterialPipelineIds::Slideshow2D;
    }
};

} // namespace com
} // namespace bl

#endif
