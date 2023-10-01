#ifndef BLIB_GRAPHICS_COMPONENTS_ANIMATION2DPLAYER_HPP
#define BLIB_GRAPHICS_COMPONENTS_ANIMATION2DPLAYER_HPP

#include <BLIB/Components/Animation2DPlayer.hpp>
#include <BLIB/ECS.hpp>
#include <BLIB/Graphics/Animation2D/AnimationData.hpp>
#include <BLIB/Graphics/Components/Textured.hpp>

namespace bl
{
namespace gfx
{
namespace bcom
{
/**
 * @brief Base class for 2d animation player components. Players are created on dedicated entities
 *        and dependencies are managed via the ECS dependency feature, ensuring that in-use players
 *        are not removed too soon. Custom classes should ensure they follow the same model
 *
 * @ingroup Graphics
 */
class Animation2DPlayer : private Textured {
public:
    /**
     * @brief Does nothing
     */
    Animation2DPlayer();

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
     * @param renderer The renderer instance
     * @param registry The ECS registry
     * @param entity The parent entity
     * @param player The entity with the player component
     */
    void create(rc::Renderer& renderer, ecs::Registry& registry, ecs::Entity entity,
                ecs::Entity player);

    /**
     * @brief Creates the animation components with a dedicated player component on this entity
     *
     * @param renderer The renderer instance
     * @param registry The ECS registry
     * @param entity The parent entity
     * @param animation The animation to use
     * @param play True to begin playing immediately, false to start paused
     * @param forceLoop True to loop the animation, false to use the animation's loop setting
     */
    void create(rc::Renderer& renderer, ecs::Registry& registry, ecs::Entity entity,
                const resource::Ref<gfx::a2d::AnimationData>& animation, bool play, bool forceLoop);

private:
    rc::Renderer* renderer;
    ecs::Registry* registry;
    ecs::Entity me;
    ecs::Entity playerEntity;
    com::Animation2DPlayer* player;

    void addPlayerDep();
    void cleanupPlayerDep();
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline com::Animation2DPlayer& Animation2DPlayer::getPlayer() { return *player; }

inline const com::Animation2DPlayer& Animation2DPlayer::getPlayer() const { return *player; }

} // namespace bcom
} // namespace gfx
} // namespace bl

#endif
