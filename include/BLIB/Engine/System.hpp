#ifndef BLIB_ENGINE_SYSTEM_HPP
#define BLIB_ENGINE_SYSTEM_HPP

#include <cstdint>
#include <mutex>

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
     *
     * @param dt Time to simulate in seconds
     * @param realDt Time elapsed in real seconds
     * @param residual Simulation time, in seconds, not yet accounted for
     * @param realResidual Real time, in seconds, not yet accounted for
     */
    virtual void update(std::mutex& stageMutex, float dt, float realDt, float residual,
                        float realResidual) = 0;

    /**
     * @brief Called once each frame immediately after OS events are processed and before update
     */
    virtual void notifyFrameStart() {}

    /**
     * @brief Optional early cleanup hook that is called when the engine is destructing prior to
     *        entities and resources all being freed
     */
    virtual void earlyCleanup() {}
};

} // namespace engine
} // namespace bl

#endif
