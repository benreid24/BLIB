#ifndef BLIB_PARTICLES_AFFECTOR_HPP
#define BLIB_PARTICLES_AFFECTOR_HPP

#include <span>

namespace bl
{
namespace pcl
{
template<typename T, typename R>
class ParticleManager;

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
     * @brief Proxy object that provides the interface for affectors to use
     */
    class Proxy {
    public:
        /**
         * @brief Returns the set of particles the affector should apply itself to
         */
        const std::span<T>& particles() const { return span; }

        /**
         * @brief Call to erase this affector
         */
        void eraseMe() { erased = true; }

    private:
        std::span<T> span;
        bool erased;

        Proxy(const std::span<T>& span)
        : span(span)
        , erased(false) {}

        void reset() { erased = false; }

        template<typename U, typename R>
        friend class ParticleManager;
    };

    /**
     * @brief Destroys the affector
     */
    virtual ~Affector() = default;

    /**
     * @brief Called once per frame to update particles
     *
     * @param proxy The proxy object containing update information
     * @param dt Simulation time elapsed since last call to update, in seconds
     * @param realDt Real time elapsed since last call to update, in seconds
     */
    virtual void update(Proxy& proxy, float dt, float realDt) = 0;
};

} // namespace pcl
} // namespace bl

#endif
