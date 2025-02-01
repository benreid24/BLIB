#ifndef BLIB_ENGINE_PLAYER_HPP
#define BLIB_ENGINE_PLAYER_HPP

#include <BLIB/Engine/World.hpp>
#include <BLIB/Input/Actor.hpp>
#include <BLIB/Render/Observer.hpp>
#include <BLIB/Containers/RefPool.hpp>
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
     * @brief Destroys the player
     */
    virtual ~Player() = default;

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
    void enterWorld(ctr::Ref<World> world);

    /**
     * @brief Creates and enters the world of the given type
     *
     * @tparam TWorld The type of world to create
     * @tparam ...TArgs Argument types to the world's constructor
     * @param ...args Arguments to the world's constructor
     * @return The newly created world
     */
    template<typename TWorld, typename... TArgs>
    ctr::Ref<World, TWorld> enterWorld(TArgs&&... args);

    /**
     * @brief Replaces the current world that this player is in with a new one
     *
     * @param world The new world to put this player in
     */
    void changeWorlds(ctr::Ref<World> world);

    /**
     * @brief Creates and enters the world of the given type, replacing the existing world
     *
     * @tparam TWorld The type of world to create
     * @tparam ...TArgs Argument types to the world's constructor
     * @param ...args Arguments to the world's constructor
     * @return The newly created world
     */
    template<typename TWorld, typename... TArgs>
    ctr::Ref<World, TWorld> changeWorlds(TArgs&&... args);

    /**
     * @brief Returns whether the player is currently in a world
     */
    bool isInWorld() const { return !worldStack.empty(); }

    /**
     * @brief Returns the world the player is in. Must be in a world
     *
     * @tparam TWorld The world type to cast to
     */
    template<typename TWorld = World>
    TWorld& getCurrentWorld();

    /**
     * @brief Exits the current world, popping it from this player's world stack
     */
    void leaveWorld();

    /**
     * @brief Leaves all worlds the player is in, emptying the world stack
     */
    void leaveAllWorlds();

protected:
    /**
     * @brief Creates the player
     *
     * @param owner The game engine instance
     * @param observer The renderer Observer for this player
     * @param actor The input system actor for this player
     */
    Player(Engine& owner, rc::Observer* observer, input::Actor* actor)
    : owner(owner)
    , observer(observer)
    , actor(actor) {}

    /**
     * @brief Called when the world that this player is in changes
     *
     * @param oldWorld The previous world. May be nullptr
     * @param newWorld The new world. May be nullptr
     */
    virtual void onWorldChange(World* oldWorld, World* newWorld);

private:
    Engine& owner;
    rc::Observer* observer;
    input::Actor* actor;
    std::stack<ctr::Ref<World>, std::vector<ctr::Ref<World>>> worldStack;

    friend class Engine;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename TWorld>
TWorld& Player::getCurrentWorld() {
    static_assert(std::is_base_of_v<World, TWorld>, "TWorld must derive from World");

#ifdef BLIB_DEBUG
    TWorld* w = dynamic_cast<TWorld*>(worldStack.top().get());
    if (!w) { BL_LOG_ERROR << "Incorrect world type cast"; }
#else
    TWorld* w = static_cast<TWorld*>(worldStack.top().get());
#endif

    return *w;
}

} // namespace engine
} // namespace bl

#endif
