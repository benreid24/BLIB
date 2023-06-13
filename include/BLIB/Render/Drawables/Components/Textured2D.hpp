#ifndef BLIB_RENDER_DRAWABLES_COMPONENTS_SCALABLE_HPP
#define BLIB_RENDER_DRAWABLES_COMPONENTS_SCALABLE_HPP

#include <BLIB/Render/Drawables/Components/Textured.hpp>
#include <BLIB/Render/Drawables/Components/Transformable.hpp>

namespace bl
{
namespace render
{
namespace draw
{
namespace base
{
/**
 * @brief Intermediate class for textured 2d objects. Contains a transform and a texture, as well as
 *        some helepr methods to easily scale for overlays
 *
 * @ingroup Renderer
 */
class Textured2D
: public Textured
, public Transformable {
public:
    /**
     * @brief Initializes to empty
     */
    Textured2D() = default;

    /**
     * @brief Scales the transform to take up a percentage of the given overlay width. Maintains
     *        aspect ratio
     *
     * @param ratio Percentage of overlay width to take up, in range [0, 1]
     * @param overlayWidth Width of the overlay
     */
    void scaleWidthToOverlay(float ratio, float overlayWidth = 1.f);

    /**
     * @brief Scales the transform to take up a percentage of the given overlay height. Maintains
     *        aspect ratio
     *
     * @param ratio Percentage of overlay height to take up, in range [0, 1]
     * @param overlayHeight Height of the overlay
     */
    void scaleHeightToOverlay(float ratio, float overlayHeigt = 1.f);

    /**
     * @brief Scales to take up a percentage of overlay width and height. Does not maintain AR
     *
     * @param ratios Width and height percentages in ranges [0, 1]
     * @param overlaySize Width and height of the overlay
     */
    void scaleToOverlay(const glm::vec2& ratios, const glm::vec2& overlaySize = {1.f, 1.f});

protected:
    /**
     * @brief Creates the transform and texture components
     *
     * @param registry The ECS registry instance
     * @param owner The ECS entity id
     * @param texture The texture to use
     */
    void create(ecs::Registry& registry, ecs::Entity owner, const res::TextureRef& texture);
};
} // namespace base
} // namespace draw
} // namespace render
} // namespace bl

#endif
