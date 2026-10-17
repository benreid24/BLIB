#ifndef BLIB_OBJECTS_2D_ANIMATION_HPP
#define BLIB_OBJECTS_2D_ANIMATION_HPP

#include <BLIB/Components/Animation2D.hpp>
#include <BLIB/Objects/2D/DrawableObject2D.hpp>
#include <BLIB/Objects/Components/Animation2DPlayer.hpp>
#include <BLIB/Objects/Components/OverlayScalable.hpp>
#include <BLIB/Objects/Components/Transform2D.hpp>

namespace bl
{
namespace obj
{
/// Contains 2d world objects
namespace o2d
{
/**
 * @brief Basic 2d animation. Supports arbitrary animations. Prefer Slideshow for supported
 *        animations as more work is done on the GPU and slideshows can be batched into larger
 *        vertex arrays while each Animation2D always requires a dedicated draw call
 *
 * @ingroup Objects
 */
class Animation
: public DrawableObject2D<com::Animation2D>
, public bcom::Animation2DPlayer {
public:
    /**
     * @brief Does nothing
     */
    Animation();

    /**
     * @brief Creates the animation with its own player
     *
     * @param world The world to create the object in
     * @param animation The animation to use
     * @param play True to play immediately, false to start paused
     * @param forceLoop True to always loop, false defer to animation setting
     */
    Animation(engine::World& world, as::TypedRef<asi::Animation2DSetPayload> animation,
              bool play = false, bool forceLoop = false);

    /**
     * @brief Creates the animation with its own player on an existing entity
     *
     * @param world The world to create the object in
     * @param existingEntity The existing entity to add components to
     * @param animation The animation to use
     * @param play True to play immediately, false to start paused
     * @param forceLoop True to always loop, false defer to animation setting
     */
    Animation(engine::World& world, ecs::Entity existingEntity,
              as::TypedRef<asi::Animation2DSetPayload> animation, bool play = false,
              bool forceLoop = false);

    /**
     * @brief Creates the animation sharing player state with the given animation
     *
     * @param world The world to create the object in
     * @param player The other animation to share player state with
     */
    Animation(engine::World& world, const Animation& player);

    /**
     * @brief Creates the animation sharing player state with the given animation
     *
     * @param world The world to create the object in
     * @param existingEntity The existing entity to add components to
     * @param player The other animation to share player state with
     */
    Animation(engine::World& world, ecs::Entity existingEntity, const Animation& player);

    /**
     * @brief Creates the animation with its own player
     *
     * @param world The world to create the object in
     * @param animation The animation to use
     * @param play True to play immediately, false to start paused
     * @param forceLoop True to always loop, false defer to animation setting
     */
    void createWithUniquePlayer(engine::World& world,
                                as::TypedRef<asi::Animation2DSetPayload> animation,
                                bool play = false, bool forceLoop = false);

    /**
     * @brief Creates the animation with its own player on an existing entity
     *
     * @param world The world to create the object in
     * @param existingEntity The existing entity to add components to
     * @param animation The animation to use
     * @param play True to play immediately, false to start paused
     * @param forceLoop True to always loop, false defer to animation setting
     */
    void createWithUniquePlayer(engine::World& world, ecs::Entity existingEntity,
                                as::TypedRef<asi::Animation2DSetPayload> animation,
                                bool play = false, bool forceLoop = false);

    /**
     * @brief Creates the animation sharing player state with the given animation
     *
     * @param world The world to create the object in
     * @param player The other animation to share player state with
     */
    void createWithSharedPlayer(engine::World& world, const Animation& player);

    /**
     * @brief Creates the animation sharing player state with the given animation
     *
     * @param world The world to create the object in
     * @param existingEntity The existing entity to add components to
     * @param player The other animation to share player state with
     */
    void createWithSharedPlayer(engine::World& world, ecs::Entity existingEntity,
                                const Animation& player);

    /**
     * @brief Sets a new animation source and creates a new dedicated player for it
     *
     * @param animation The new animation to render
     * @param play True to play immediately, false to start paused
     * @param forceLoop True to always loop, false defer to animation setting
     */
    void setAnimationWithUniquePlayer(as::TypedRef<asi::Animation2DSetPayload> animation, bool play,
                                      bool forceLoop);

    /**
     * @brief Helper method to set the scale so that the entity is a certain size
     *
     * @param size The size to scale to
     */
    virtual void scaleToSize(const glm::vec2& size) override;

protected:
    /**
     * @brief Called when the local size is queried
     */
    virtual void ensureLocalSizeUpdated() override;
};

} // namespace o2d
} // namespace obj
} // namespace bl

#endif
