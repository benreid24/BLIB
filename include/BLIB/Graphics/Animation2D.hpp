#ifndef BLIB_GRAPHICS_ANIMATION2D_HPP
#define BLIB_GRAPHICS_ANIMATION2D_HPP

#include <BLIB/Components/Animation2D.hpp>
#include <BLIB/Graphics/Animation2D/AnimationData.hpp>
#include <BLIB/Graphics/Components/Animation2DPlayer.hpp>
#include <BLIB/Graphics/Components/OverlayScalable.hpp>
#include <BLIB/Graphics/Components/Transform2D.hpp>
#include <BLIB/Graphics/Drawable.hpp>

namespace bl
{
namespace gfx
{
/**
 * @brief Basic 2d animation. Supports arbitrary animations. Prefer Slideshow for supported
 *        animations as more work is done on the GPU and slideshows can be batched into larger
 *        vertex arrays while each Animation2D always requires a dedicated draw call
 *
 * @ingroup Graphics
 */
class Animation2D
: public Drawable<com::Animation2D>
, public bcom::OverlayScalable
, public bcom::Animation2DPlayer {
public:
    /**
     * @brief Does nothing
     */
    Animation2D();

    /**
     * @brief Creates the animation with its own player
     *
     * @param world The world to create the object in
     * @param animation The animation to use
     * @param play True to play immediately, false to start paused
     * @param forceLoop True to always loop, false defer to animation setting
     */
    Animation2D(engine::World& world, const resource::Ref<a2d::AnimationData>& animation,
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
    Animation2D(engine::World& world, ecs::Entity existingEntity,
                const resource::Ref<a2d::AnimationData>& animation, bool play = false,
                bool forceLoop = false);

    /**
     * @brief Creates the animation sharing player state with the given animation
     *
     * @param world The world to create the object in
     * @param player The other animation to share player state with
     */
    Animation2D(engine::World& world, const Animation2D& player);

    /**
     * @brief Creates the animation sharing player state with the given animation
     *
     * @param world The world to create the object in
     * @param existingEntity The existing entity to add components to
     * @param player The other animation to share player state with
     */
    Animation2D(engine::World& world, ecs::Entity existingEntity, const Animation2D& player);

    /**
     * @brief Creates the animation with its own player
     *
     * @param world The world to create the object in
     * @param animation The animation to use
     * @param play True to play immediately, false to start paused
     * @param forceLoop True to always loop, false defer to animation setting
     */
    void createWithUniquePlayer(engine::World& world,
                                const resource::Ref<a2d::AnimationData>& animation,
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
                                const resource::Ref<a2d::AnimationData>& animation,
                                bool play = false, bool forceLoop = false);

    /**
     * @brief Creates the animation sharing player state with the given animation
     *
     * @param world The world to create the object in
     * @param player The other animation to share player state with
     */
    void createWithSharedPlayer(engine::World& world, const Animation2D& player);

    /**
     * @brief Creates the animation sharing player state with the given animation
     *
     * @param world The world to create the object in
     * @param existingEntity The existing entity to add components to
     * @param player The other animation to share player state with
     */
    void createWithSharedPlayer(engine::World& world, ecs::Entity existingEntity,
                                const Animation2D& player);

    /**
     * @brief Sets a new animation source and creates a new dedicated player for it
     *
     * @param animation The new animation to render
     * @param play True to play immediately, false to start paused
     * @param forceLoop True to always loop, false defer to animation setting
     */
    void setAnimationWithUniquePlayer(const resource::Ref<a2d::AnimationData>& animation, bool play,
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

} // namespace gfx
} // namespace bl

#endif
