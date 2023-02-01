#ifndef BLIB_RENDER_DRAWABLES_DRAWABLE_HPP
#define BLIB_RENDER_DRAWABLES_DRAWABLE_HPP

#include <BLIB/Render/Overlays/OverlayRenderContext.hpp>
#include <glad/vulkan.h>

namespace bl
{
namespace render
{
/// Collection of SFML-like primitives that can be drawn in overlays
namespace overlay
{
/**
 * @brief Base class for scene-independent objects. Drawables may have more complex rendering logic,
 *        such as binding their own pipelines, manipulating the viewport, etc. All drawables are
 *        rendered during an Overlay render pass and must be compatible with it
 *
 * @ingroup Renderer
 */
class Drawable {
public:
    /**
     * @brief Destroys the drawable
     */
    virtual ~Drawable() = default;

    /**
     * @brief Sets whether or not this drawable will be hidden
     *
     * @param hidden True to skip rendering, false to render
     */
    void setHidden(bool hidden);

    /**
     * @brief Issues the commands to render this drawable
     *
     * @param context Settings to render with
     */
    void render(OverlayRenderContext& context);

protected:
    /**
     * @brief Initializes the drawable
     */
    Drawable();

    /**
     * @brief Renders the drawable by issuing commands into the given command buffer
     *
     * @param context Render context containing the settings to render with
     */
    virtual void doRender(OverlayRenderContext& context) = 0;

private:
    bool hidden;
};

} // namespace overlay
} // namespace render
} // namespace bl

#endif
