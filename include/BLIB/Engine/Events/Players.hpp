#ifndef BLIB_ENGINE_EVENTS_PLAYERS_HPP
#define BLIB_ENGINE_EVENTS_PLAYERS_HPP

namespace bl
{
namespace engine
{
class Player;

namespace event
{
/**
 * @brief Fired when a player is added to the engine
 *
 * @ingroup EngineEvents
 */
struct PlayerAdded {
    /// The new player
    Player& player;
};

/**
 * @brief Fired when a player is removed from the engine
 *
 * @ingroup EngineEvents
 */
struct PlayerRemoved {
    /// The player about to be removed
    Player& player;
};

} // namespace event
} // namespace engine
} // namespace bl

#endif
