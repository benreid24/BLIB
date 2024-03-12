#ifndef BLIB_PARTICLES_PARTICLEMANAGER_HPP
#define BLIB_PARTICLES_PARTICLEMANAGER_HPP

#include <BLIB/Particles/Affector.hpp>
#include <BLIB/Particles/Emitter.hpp>
#include <BLIB/Particles/ParticleManagerBase.hpp>
#include <BLIB/Particles/Renderer.hpp>
#include <BLIB/Particles/Sink.hpp>
#include <memory>

namespace bl
{
namespace pcl
{
/**
 * @brief Particle manager for a specific type of particle. Instances are owned by the
 *        ParticleSystem and should be obtained from there
 *
 * @tparam T The type of particle to manage
 * @tparam TRenderer The type of renderer plugin to use
 * @ingroup Particles
 */
template<typename T, typename TRenderer = Renderer<T>>
class ParticleManager : public ParticleManagerBase {
public:
    using TAffector = Affector<T>;
    using TEmitter  = Emitter<T>;
    using TSink     = Sink<T>;

    /**
     * @brief Destroys the particle manager
     */
    virtual ~ParticleManager() = default;

    /**
     * @brief Updates all the particles
     *
     * @param dt Elapsed simulation time in seconds
     * @param realDt Elapsed real time in seconds
     */
    virtual void update(float dt, float realDt) override;

    /**
     * @brief Adds a particle affector of the given type
     *
     * @tparam U The type of affector to add
     * @param ...args Arguments to construct the affector with
     * @return A pointer to the new affector
     */
    template<typename U, template... TArgs>
    U* addAffector(TArgs&&... args);

    /**
     * @brief Fetches the affector of the given type. Returns the first one found if there are
     *        multiple of the same type
     *
     * @tparam U The type of affector to fetch
     * @return A pointer to the affector, nullptr if not found
     */
    template<typename U>
    U* getAffector();

    /**
     * @brief Removes the particular affector from the manager
     *
     * @param affector The affector to remove
     */
    void removeAffector(TAffector* affector);

    /**
     * @brief Removes all affectors of the given type from the manager
     *
     * @tparam U The affector type to remove
     */
    template<typename U>
    void removeAffectors();

    /**
     * @brief Removes all particle affectors
     */
    void removeAllAffectors();

    /**
     * @brief Adds a particle emitter of the given type
     *
     * @tparam U The type of emitter to add
     * @param ...args Arguments to construct the emitter with
     * @return A pointer to the new emitter
     */
    template<typename U, template... TArgs>
    U* addEmitter(TArgs&&... args);

    /**
     * @brief Fetches the emitter of the given type. Returns the first one found if there are
     *        multiple of the same type
     *
     * @tparam U The type of emitter to fetch
     * @return A pointer to the emitter, nullptr if not found
     */
    template<typename U>
    U* getEmitter();

    /**
     * @brief Removes the particular emitter from the manager
     *
     * @param emitter The emitter to remove
     */
    void removeEmitter(TEmitter* emitter);

    /**
     * @brief Removes all emitters of the given type from the manager
     *
     * @tparam U The emitter type to remove
     */
    template<typename U>
    void removeEmitters();

    /**
     * @brief Removes all particle emitters
     */
    void removeAllEmitters();

    /**
     * @brief Adds a particle sink of the given type
     *
     * @tparam U The type of sink to add
     * @param ...args Arguments to construct the sink with
     * @return A pointer to the new sink
     */
    template<typename U, template... TArgs>
    U* addSink(TArgs&&... args);

    /**
     * @brief Fetches the sink of the given type. Returns the first one found if there are
     *        multiple of the same type
     *
     * @tparam U The type of sink to fetch
     * @return A pointer to the sink, nullptr if not found
     */
    template<typename U>
    U* getSink();

    /**
     * @brief Removes the particular sink from the manager
     *
     * @param sink The sink to remove
     */
    void removeSink(TSink* sink);

    /**
     * @brief Removes all sinks of the given type from the manager
     *
     * @tparam U The sink type to remove
     */
    template<typename U>
    void removeSinks();

    /**
     * @brief Removes all particle sinks
     */
    void removeAllSinks();

    /**
     * @brief Destroys all particles, affectors, emitters, and sinks
     */
    void clearAndReset();

    // TODO - renderer interface

private:
    std::mutex mutex;
    std::vector<T> particles;
    std::vector<std::size_t> freeList;
};

} // namespace pcl
} // namespace bl

#endif
