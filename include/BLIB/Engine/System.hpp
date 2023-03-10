#ifndef BLIB_ENGINE_SYSTEM_HPP
#define BLIB_ENGINE_SYSTEM_HPP

#include <cstdint>

namespace bl
{
namespace engine
{
class Engine;

/**
 * @brief Base class for Engine systems which can be registered in the Systems registry
 *
 * @ingroup Engine
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
     * @param stageMutex Reference to a mutex that can be used to synchronize systems in the same
     *                   stage that run in parallel
     * @param dt Time to simulate in seconds
     */
    virtual void update(std::mutex& stageMutex, float dt) = 0;
};

} // namespace engine
} // namespace bl

#endif
