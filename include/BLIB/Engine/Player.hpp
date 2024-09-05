#ifndef BLIB_ENGINE_PLAYER_HPP
#define BLIB_ENGINE_PLAYER_HPP

#include <BLIB/Input/Actor.hpp>
#include <BLIB/Render/Observer.hpp>

namespace bl
{
namespace engine
{
class Engine;

/**
 * @brief Engine level concept of a player. Wraps renderer Observer and input system Actor
 *
 * @ingroup Engine
 */
class Player {
public:
    /**
     * @brief Returns the Observer for this player
     */
    rc::Observer& getRenderObserver() { return *observer; }

    /**
     * @brief Returns the Observer for this player
     */
    const rc::Observer& getRenderObserver() const { return *observer; }

    /**
     * @brief Returns the input Actor for this player
     */
    input::Actor& getInputActor() { return *actor; }

    /**
     * @brief Returns the input Actor for this player
     */
    const input::Actor& getInputActor() const { return *actor; }

private:
    rc::Observer* observer;
    input::Actor* actor;

    Player(rc::Observer* observer, input::Actor* actor)
    : observer(observer)
    , actor(actor) {}

    friend class Engine;
};

} // namespace engine
} // namespace bl

#endif
