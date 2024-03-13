#ifndef BLIB_PARTICLES_SINK_HPP
#define BLIB_PARTICLES_SINK_HPP

#include <mutex>
#include <span>
#include <vector>

namespace bl
{
namespace pcl
{
template<typename T, typename R>
class ParticleManager;

/**
 * @brief Base class for all particle sinks. Provides the interface used by ParticleManagers
 *
 * @tparam T The type of particle the sink works with
 * @ingroup Particles
 */
template<typename T>
class Sink {
public:
    /**
     * @brief Proxy interface for sinks to destroy particles
     */
    class Proxy {
    public:
        /**
         * @brief Call to destroy the given particle
         * @param particle The particle to destroy
         */
        void destroy(const T& particle) const {
            const std::size_t i = particle - base;

            std::unique_lock lock(mutex);
            if (!freed[i]) {
                freed[i] = true;
                freeList.emplace_back(i);
            }
        }

    private:
        std::mutex& mutex;
        T* base;
        std::vector<std::size_t>& freeList;
        std::vector<bool>& freed;

        Proxy(std::mutex& mutex, T* base, std::vector<std::size_t>& freeList,
              std::vector<bool>& freed)
        : mutex(mutex)
        , base(base)
        , freeList(freeList)
        , freed(freed) {}

        template<typename R>
        friend class ParticleManager<T, R>;
    };

    /**
     * @brief Destroys the Sink
     */
    virtual ~Sink() = default;

    /**
     * @brief Called once per frame. Sinks should destroy particles in here
     *
     * @param proxy The proxy object to destroy particles with
     * @param particles The set of particles to scan and destroy from
     * @param dt Elapsed simulation time, in seconds, since last call to update
     * @param realDt Elapsed real time, in seconds, since last call to update
     */
    virtual void update(const Proxy& proxy, std::span<T> particles, float dt, float realDt) = 0;
};

} // namespace pcl
} // namespace bl

#endif
