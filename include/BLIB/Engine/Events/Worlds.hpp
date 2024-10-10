#ifndef BLIB_ENGINE_EVEVNTS_WORLDS_HPP
#define BLIB_ENGINE_EVEVNTS_WORLDS_HPP

namespace bl
{
namespace engine
{
class World;

namespace event
{
/**
 * @brief Fired when a world is created
 *
 * @ingroup Engine
 */
struct WorldCreated {
    World& world;
};

/**
 * @brief Fired just before a world is destroyed
 */
struct WorldDestroyed {
    World& world;
};

} // namespace event
} // namespace engine
} // namespace bl

#endif
