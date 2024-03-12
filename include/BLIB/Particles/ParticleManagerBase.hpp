#ifndef BLIB_PARTICLES_PARTICLEMANAGERBASE_HPP
#define BLIB_PARTICLES_PARTICLEMANAGERBASE_HPP

namespace bl
{
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
     * @brief Updates all the particles in the particle manager
     *
     * @param dt Elapsed simulation time in seconds
     * @param realDt Elapsed real time in seconds
     */
    virtual void update(float dt, float realDt) = 0;

    // TODO - renderer interface
};

} // namespace pcl
} // namespace bl

#endif
