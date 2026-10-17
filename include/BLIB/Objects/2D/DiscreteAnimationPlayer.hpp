#ifndef BLIB_OBJECTS_2D_DISCRETEANIMATIONPLAYER_HPP
#define BLIB_OBJECTS_2D_DISCRETEANIMATIONPLAYER_HPP

#include <BLIB/Components/Animation2DPlayer.hpp>
#include <BLIB/ECS/EntityBacked.hpp>

namespace bl
{
namespace obj
{
namespace o2d
{
/**
 * @brief Utility class to manage a discrete animation player separate from a graphics entity
 *
 * @ingroup Objects
 */
class DiscreteAnimationPlayer : public ecs::EntityBacked {
public:
    /// The type of animation to create the player for
    enum Mode { Slideshow, VertexAnimation, Auto };

    /**
     * @brief Does nothing
     */
    DiscreteAnimationPlayer();

    /**
     * @brief Creates the animation player entity and component
     *
     * @param world The world to create the object in
     * @param animation The animation data to use
     * @param mode The type of player to create
     * @param play True to play immediately, false to create paused
     * @param forceLoop True to loop the animation, false to defer to the source file
     */
    void create(engine::World& world, as::TypedRef<asi::Animation2DSetPayload> animation, Mode mode,
                bool play = false, bool forceLoop = false);

    /**
     * @brief Access the animation player
     */
    com::Animation2DPlayer& getPlayer() { return *player; }

    /**
     * @brief Access the animation player
     */
    const com::Animation2DPlayer& getPlayer() const { return *player; }

private:
    com::Animation2DPlayer* player;
};

} // namespace o2d
} // namespace obj
} // namespace bl

#endif
