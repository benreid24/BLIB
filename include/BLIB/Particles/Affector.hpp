#ifndef BLIB_PARTICLES_AFFECTOR_HPP
#define BLIB_PARTICLES_AFFECTOR_HPP

#include <span>

namespace bl
{
namespace pcl
{
/**
 * @brief Base class for all particle affectors. Provides the interface used by ParticleManagers
 *
 * @tparam T The type of particle the affector works with
 * @ingroup Particles
 */
template<typename T>
class Affector {
public:
    /**
     * @brief Destroys the affector
     */
    virtual ~Affector() = default;

    /**
     * @brief Called once per frame to update particles
     *
     * @param particles The set of particles to update
     * @param dt Simulation time elapsed since last call to update, in seconds
     * @param realDt Real time elapsed since last call to update, in seconds
     */
    virtual void update(std::span<T> particles, float dt, float realDt) = 0;
};

} // namespace pcl
} // namespace bl

#endif
