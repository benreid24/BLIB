#ifndef BLIB_COMPONENTS_ANIMATION2D_HPP
#define BLIB_COMPONENTS_ANIMATION2D_HPP

#include <BLIB/Components/Animation2DPlayer.hpp>
#include <BLIB/Render/Buffers/IndexBuffer.hpp>
#include <BLIB/Render/Components/DrawableBase.hpp>

namespace bl
{
namespace engine
{
class Engine;
}

namespace com
{
/**
 * @brief Component for non-slideshow 2d animations
 *
 * @ingroup Components
 */
struct Animation2D : public rc::rcom::DrawableBase {
    void* systemHandle;
    const Animation2DPlayer* player;

    /**
     * @brief Does nothing
     */
    Animation2D();

    /**
     * @brief Creates the index buffer to render the animation. Does not register ECS dependency.
     *        The player must remain valid while the animation exists
     *
     * @param engine The game engine instance
     * @param anim The animation to use when creating the vertices
     */
    void create(engine::Engine& engine, const Animation2DPlayer& anim);
};

} // namespace com
} // namespace bl

#endif
