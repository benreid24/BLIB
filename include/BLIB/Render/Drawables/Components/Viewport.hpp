#ifndef BLIB_RENDER_DRAWABLES_VIEWPORT_HPP
#define BLIB_RENDER_DRAWABLES_VIEWPORT_HPP

#include <BLIB/ECS.hpp>
#include <BLIB/Render/Overlays/Viewport.hpp>

namespace bl
{
namespace render
{
namespace draw
{
namespace base
{
/**
 * @brief Base component class for drawables that can have viewports for overlays
 *
 * @ingroup Renderer
 */
class Viewport {
public:
    /**
     * @brief Does nothing
     */
    Viewport() = default;

    /**
     * @brief Creates or updates the viewport of this drawable
     *
     * @param viewport The viewport to constrain rendering to
     */
    void setViewport(const ovy::Viewport& viewport);

    /**
     * @brief Removes any viewport for this drawable
     */
    void clearViewport();

protected:
    /**
     * @brief Sets the ECS entity id for viewport creation. Does not add a viewport
     *
     * @param The ECS registry instance
     * @param entity The ECS entity id
     */
    void create(ecs::Registry& registry, ecs::Entity entity);

private:
    ecs::Registry* registry;
    ecs::Entity ecsId;
};

} // namespace base
} // namespace draw
} // namespace render
} // namespace bl

#endif
