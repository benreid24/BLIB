#ifndef BLIB_PARTICLES_METAUPDATER_HPP
#define BLIB_PARTICLES_METAUPDATER_HPP

#include <BLIB/Particles/Affector.hpp>
#include <BLIB/Particles/Emitter.hpp>
#include <BLIB/Particles/Sink.hpp>
#include <memory>
#include <vector>

namespace bl
{
namespace pcl
{
template<typename T>
class ParticleManager;

/**
 * @brief Generic plugin base for update logic to be applied to particle systems prior to sinks,
 *        emitters, and affectors. Meta-updaters can be used to manipulate the particle manager
 *
 * @tparam T The particle type
 * @ingroup Particles
 */
template<typename T>
class MetaUpdater {
public:
    class Proxy {
    public:
        /**
         * @brief Call to erase this sink
         */
        void eraseMe() { erased = true; }

        /**
         * @brief Access the particle manager that owns this sink
         */
        ParticleManager<T>& getManager() const { return manager; }

        /**
         * @brief Returns the full list of current affectors in the particle system
         */
        const std::vector<std::unique_ptr<Affector<T>>>& getAffectors() const { return affectors; }

        /**
         * @brief Returns the full list of current emitters in the particle system
         */
        const std::vector<std::unique_ptr<Emitter<T>>>& getEmitters() const { return emitters; }

        /**
         * @brief Returns the full list of current sinks in the particle system
         */
        const std::vector<std::unique_ptr<Sink<T>>>& getSinks() const { return sinks; }

    private:
        ParticleManager<T>& manager;
        const std::vector<std::unique_ptr<Affector<T>>>& affectors;
        const std::vector<std::unique_ptr<Emitter<T>>>& emitters;
        const std::vector<std::unique_ptr<Sink<T>>>& sinks;
        bool erased;

        Proxy(ParticleManager<T>& manager,
              const std::vector<std::unique_ptr<Affector<T>>>& affectors,
              const std::vector<std::unique_ptr<Emitter<T>>>& emitters,
              const std::vector<std::unique_ptr<Sink<T>>>& sinks)
        : manager(manager)
        , affectors(affectors)
        , emitters(emitters)
        , sinks(sinks)
        , erased(false) {}

        void reset() { erased = false; }

        friend class ParticleManager<T>;
    };

    /**
     * @brief Destroys the updater
     */
    virtual ~MetaUpdater() = default;

    /**
     * @brief Called once per frame before emitters, sinks, and affectors
     *
     * @param proxy The proxy object to interface with
     * @param dt Elapsed simulation time in seconds
     * @param realDt Elapsed real time in seconds
     */
    virtual void update(Proxy& proxy, float dt, float realDt) = 0;
};

} // namespace pcl
} // namespace bl

#endif
