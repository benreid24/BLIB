#ifndef BLIB_ENGINE_PLAYER_HPP
#define BLIB_ENGINE_PLAYER_HPP

#include <BLIB/Engine/World.hpp>
#include <BLIB/Input/Actor.hpp>
#include <BLIB/Render/Observer.hpp>
#include <BLIB/Util/RefPool.hpp>
#include <stack>
#include <vector>

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

    /**
     * @brief Enters this player to the given world, adding it to the world stack for this player
     *
     * @param world The world to enter
     */
    void enterWorld(util::Ref<World> world);

    /**
     * @brief Creates and enters the world of the given type
     *
     * @tparam TWorld The type of world to create
     * @tparam ...TArgs Argument types to the world's constructor
     * @param ...args Arguments to the world's constructor
     * @return The newly created world
     */
    template<typename TWorld, typename... TArgs>
    util::Ref<World, TWorld> enterWorld(TArgs&&... args);

    /**
     * @brief Replaces the current world that this player is in with a new one
     *
     * @param world The new world to put this player in
     */
    void changeWorlds(util::Ref<World> world);

    /**
     * @brief Creates and enters the world of the given type, replacing the existing world
     *
     * @tparam TWorld The type of world to create
     * @tparam ...TArgs Argument types to the world's constructor
     * @param ...args Arguments to the world's constructor
     * @return The newly created world
     */
    template<typename TWorld, typename... TArgs>
    util::Ref<World, TWorld> changeWorlds(TArgs&&... args);

    /**
     * @brief Exits the current world, popping it from this player's world stack
     */
    void leaveWorld();

    /**
     * @brief Leaves all worlds the player is in, emptying the world stack
     */
    void leaveAllWorlds();

private:
    Engine& owner;
    rc::Observer* observer;
    input::Actor* actor;
    std::stack<util::Ref<World>, std::vector<util::Ref<World>>> worldStack;

    Player(Engine& owner, rc::Observer* observer, input::Actor* actor)
    : owner(owner)
    , observer(observer)
    , actor(actor) {}

    friend class Engine;
};

} // namespace engine
} // namespace bl

#endif
