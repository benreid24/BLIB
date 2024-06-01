#ifndef BLIB_PARTICLES_PARTICLEMANAGER_HPP
#define BLIB_PARTICLES_PARTICLEMANAGER_HPP

#include <BLIB/Particles/Affector.hpp>
#include <BLIB/Particles/Emitter.hpp>
#include <BLIB/Particles/GlobalParticleSystemInfo.hpp>
#include <BLIB/Particles/MetaUpdater.hpp>
#include <BLIB/Particles/ParticleManagerBase.hpp>
#include <BLIB/Particles/RenderTypeMap.hpp>
#include <BLIB/Particles/Sink.hpp>
#include <cmath>
#include <iterator>
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
template<typename T>
class ParticleManager : public ParticleManagerBase {
public:
    using TRenderer = typename RenderTypeMap<T>::TRenderer;
    using TUpdater  = MetaUpdater<T>;
    using TAffector = Affector<T>;
    using TEmitter  = Emitter<T>;
    using TSink     = Sink<T>;

    /**
     * @brief Initializes the particle manager
     */
    ParticleManager();

    /**
     * @brief Called once after being constructed
     *
     * @param engine The game engine instance
     */
    virtual void init(engine::Engine& engine) override;

    /**
     * @brief Destroys the particle manager
     */
    virtual ~ParticleManager() = default;

    /**
     * @brief Updates all the particles
     *
     * @param threadPool The threadpool to use
     * @param dt Elapsed simulation time in seconds
     * @param realDt Elapsed real time in seconds
     */
    virtual void update(util::ThreadPool& threadPool, float dt, float realDt) override;

    /**
     * @brief Adds a particle updater of the given type
     *
     * @tparam U The type of updater to add
     * @param ...args Arguments to construct the updater with
     * @return A pointer to the new updater
     */
    template<typename U, typename... TArgs>
    U* addUpdater(TArgs... args);

    /**
     * @brief Fetches the updater of the given type. Returns the first one found if there are
     *        multiple of the same type
     *
     * @tparam U The type of updater to fetch
     * @return A pointer to the updater, nullptr if not found
     */
    template<typename U>
    U* getUpdater();

    /**
     * @brief Removes the particular updater from the manager
     *
     * @param updater The updater to remove
     */
    void removeUpdater(TUpdater* updater);

    /**
     * @brief Removes all updaters of the given type from the manager
     *
     * @tparam U The updater type to remove
     */
    template<typename U>
    void removeUpdaters();

    /**
     * @brief Removes all particle updaters
     */
    void removeAllUpdaters();

    /**
     * @brief Adds a particle affector of the given type
     *
     * @tparam U The type of affector to add
     * @param ...args Arguments to construct the affector with
     * @return A pointer to the new affector
     */
    template<typename U, typename... TArgs>
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
    template<typename U, typename... TArgs>
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
    template<typename U, typename... TArgs>
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
    virtual void clearAndReset() override;

    /**
     * @brief Called when the particle manager should be rendered in the given scene
     *
     * @param scene The scene to add to
     */
    virtual void addToScene(rc::Scene* scene) override;

    /**
     * @brief Called when the particles should be removed from its current scene
     */
    virtual void removeFromScene() override;

    /**
     * @brief Returns the renderer for this particle manager
     */
    TRenderer& getRenderer();

    /**
     * @brief Returns the renderer for this particle manager
     */
    const TRenderer& getRenderer() const;

    /**
     * @brief Returns the current number of active particles in this manager. Safe to call from
     *        update() methods. External callers should use the locked version
     */
    std::size_t getParticleCount() const;

    /**
     * @brief Returns the current number of active particles. Use this version from outside the
     *        particle system and its emitters, affectors, sinks, and updaters
     */
    std::size_t getParticleCountLocked() const;

private:
    static constexpr std::size_t ParticlesPerThread = 800;

    mutable std::mutex mutex;
    engine::Engine* engine;
    TRenderer renderer;
    priv::GlobalParticleSystemInfo globalInfo;
    std::vector<T> particles;

    std::mutex releaseMutex;
    std::vector<std::size_t> freeList;
    std::vector<bool> freed;
    std::vector<std::future<void>> futures;

    std::vector<std::unique_ptr<TUpdater>> updaters;
    std::vector<std::unique_ptr<TAffector>> affectors;
    std::vector<std::unique_ptr<TEmitter>> emitters;
    std::vector<std::unique_ptr<TSink>> sinks;

    void updateSink(std::size_t i, util::ThreadPool& pool, float dt, float realDt);
};

} // namespace pcl
} // namespace bl

#endif
