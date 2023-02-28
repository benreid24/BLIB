#ifndef BLIB_ECS_SYSTEM_HPP
#define BLIB_ECS_SYSTEM_HPP

#include <cstdint>

namespace bl
{
namespace engine
{
class Engine;
}

namespace ecs
{
/**
 * @brief Base class for ECS systems which can be registered in the engine::Systems registry
 *
 * @ingroup ECS
 */
class System {
public:
    /**
     * @brief Destroys the system
     */
    virtual ~System() = default;

    /**
     * @brief This is called once on engine startup
     *
     * @param engine The game engine instance
     */
    virtual void init(engine::Engine& engine) = 0;

    /**
     * @brief This is called each frame with the time to simulate
     *
     * @param dt Time to simulate in seconds
     */
    virtual void update(float dt) = 0;
};

} // namespace ecs
} // namespace bl

#endif
