#ifndef BLIB_PARTICLES_PARTICLEMANAGERBASE_HPP
#define BLIB_PARTICLES_PARTICLEMANAGERBASE_HPP

#include <BLIB/Util/ThreadPool.hpp>

namespace bl
{
namespace engine
{
class Engine;
}
namespace rc
{
class Scene;
}

namespace pcl
{
/**
 * @brief Base class for particle managers. Particle managers own and
 *        manage specific particle types
 *
 * @ingroup Particles
 */
class ParticleManagerBase {
public:
    /**
     * @brief Destroys the particle manager
     */
    virtual ~ParticleManagerBase() = default;

    /**
     * @brief Called once after being constructed
     *
     * @param engine The game engine instance
     */
    virtual void init(engine::Engine& engine) = 0;

    /**
     * @brief Updates all the particles in the particle manager
     *
     * @param threadPool The threadpool to use
     * @param dt Elapsed simulation time in seconds
     * @param realDt Elapsed real time in seconds
     */
    virtual void update(util::ThreadPool& threadPool, float dt, float realDt) = 0;

    /**
     * @brief Destroys all particles, affectors, emitters, and sinks
     */
    virtual void clearAndReset() = 0;

    /**
     * @brief Called when the particle manager should be rendered in the given scene. May be called
     *        multiple times in order for multiple observers to view the same particles
     *
     * @param scene The scene to add to
     */
    virtual void addToScene(rc::Scene* scene) = 0;

    /**
     * @brief Called when the particles should be removed from its current scene
     */
    virtual void removeFromScene() = 0;
};

} // namespace pcl
} // namespace bl

#endif
