#ifndef BLIB_GRAPHICS_COMPONENTS_HPP
#define BLIB_GRAPHICS_COMPONENTS_HPP

#include <BLIB/Components/Animation2D.hpp>
#include <BLIB/Components/Animation2DPlayer.hpp>
#include <BLIB/ECS.hpp>

namespace bl
{
namespace gfx
{
namespace base
{
/**
 * @brief Base class for 2d animation drawables
 *
 * @ingroup Graphics
 */
class Animation2D {
public:
    /**
     * @brief Does nothing
     */
    Animation2D() = default;

    /**
     * @brief Configures the animation to use the existing player in order to share play state with
     *        other animations
     *
     * @param player The entity id that has the Animation2DPlayer component to use
     */
    void useExistingPlayer(ecs::Entity player);

    /**
     * @brief Creates a new Animation2DPlayer component for this animation to have unique play state
     *
     * @param animation The animation to use
     * @param play True to begin playing immediately, false to start paused
     * @param forceLoop True to loop the animation, false to use the animation's loop setting
     */
    void createNewPlayer(const resource::Ref<gfx::a2d::AnimationData>& animation, bool play = false,
                         bool forceLoop = false);

    /**
     * @brief Returns the animation player for this animation
     */
    com::Animation2DPlayer& getPlayer();

    /**
     * @brief Returns the animation player for this animation
     */
    const com::Animation2DPlayer& getPlayer() const;

protected:
    /**
     * @brief Creates the animation components. Uses a shared player component
     *
     * @param registry The ECS registry
     * @param entity The parent entity
     * @param player The entity with the player component
     */
    void create(ecs::Registry& registry, ecs::Entity entity, ecs::Entity player);

    /**
     * @brief Creates the animation components with a dedicated player component on this entity
     *
     * @param registry The ECS registry
     * @param entity The parent entity
     * @param animation The animation to use
     * @param play True to begin playing immediately, false to start paused
     * @param forceLoop True to loop the animation, false to use the animation's loop setting
     */
    void create(ecs::Registry& registry, ecs::Entity entity,
                const resource::Ref<gfx::a2d::AnimationData>& animation, bool play, bool forceLoop);

private:
    ecs::Registry* registry;
    ecs::Entity me;
    com::Animation2D* animation;
    com::Animation2DPlayer* player;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline com::Animation2DPlayer& Animation2D::getPlayer() { return *player; }

inline const com::Animation2DPlayer& Animation2D::getPlayer() const { return *player; }

} // namespace base
} // namespace gfx
} // namespace bl

#endif
