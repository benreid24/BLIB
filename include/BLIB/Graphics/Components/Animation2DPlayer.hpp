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
class Animation2DPlayer : public Textured {
public:
    /**
     * @brief Does nothing
     *
     * @param forSlideshow Whether or not this player will be used for slideshow animations
     */
    Animation2DPlayer(bool forSlideshow);

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

    /**
     * @brief Returns the ECS entity id of the player
     */
    ecs::Entity getPlayerEntity() const;

protected:
    /**
     * @brief Creates the animation components. Uses a shared player component
     *
     * @param world The world to create the player in
     * @param entity The parent entity
     * @param player The entity with the player component
     * @param material The material of the entity
     */
    void create(engine::World& world, ecs::Entity entity, ecs::Entity player,
                com::MaterialInstance& material);

    /**
     * @brief Creates the animation components with a dedicated player component on this entity
     *
     * @param world The world to create the player in
     * @param entity The parent entity
     * @param material The material of the entity
     * @param animation The animation to use
     * @param play True to begin playing immediately, false to start paused
     * @param forceLoop True to loop the animation, false to use the animation's loop setting
     */
    void create(engine::World& world, ecs::Entity entity, com::MaterialInstance& material,
                const resource::Ref<gfx::a2d::AnimationData>& animation, bool play, bool forceLoop);

private:
    const bool forSlideshow;
    engine::World* world;
    ecs::Entity me;
    ecs::Entity playerEntity;
    com::Animation2DPlayer* player;

    void addPlayerDep();
    bool cleanupPlayerDep();
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline com::Animation2DPlayer& Animation2DPlayer::getPlayer() { return *player; }

inline const com::Animation2DPlayer& Animation2DPlayer::getPlayer() const { return *player; }

inline ecs::Entity Animation2DPlayer::getPlayerEntity() const { return playerEntity; }

} // namespace bcom
} // namespace gfx
} // namespace bl

#endif
