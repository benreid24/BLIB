#ifndef BLIB_PARTICLES_PARTICLEMANAGER_HPP
#define BLIB_PARTICLES_PARTICLEMANAGER_HPP

#include <BLIB/Particles/Affector.hpp>
#include <BLIB/Particles/Emitter.hpp>
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
    TRenderer renderer;
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

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
ParticleManager<T>::ParticleManager() {
    constexpr std::size_t DefaultCapacity = 512;

    particles.reserve(DefaultCapacity);
    freeList.reserve(DefaultCapacity / 16);
    freed.reserve(DefaultCapacity);
}

template<typename T>
void ParticleManager<T>::init(engine::Engine& engine) {
    renderer.init(engine);
}

template<typename T>
void ParticleManager<T>::update(util::ThreadPool& threadPool, float dt, float realDt) {
    std::unique_lock lock(mutex);

    // call updaters before anything else
    if (!updaters.empty()) {
        typename TUpdater::Proxy proxy(*this, affectors, emitters, sinks);
        for (std::size_t i = updaters.size(); i > 0; --i) {
            const std::size_t idx = i - 1;
            updaters[idx]->update(proxy, dt, realDt);
            if (proxy.erased) {
                if (i != updaters.size()) { updaters[idx] = std::move(updaters.back()); }
                updaters.pop_back();
            }
            proxy.reset();
        }
    }

    if (!sinks.empty() && !particles.empty()) {
        // reset free list
        freeList.clear();
        freed.resize(particles.size());
        std::fill(freed.begin(), freed.end(), false);

        // update sinks first to populate freelist
        for (std::size_t i = sinks.size(); i > 0; --i) {
            updateSink(i - 1, threadPool, dt, realDt);
        }
    }

    // run emitters to fill holes
    if (!emitters.empty()) {
        typename TEmitter::Proxy proxy(*this, particles, freeList);
        for (std::size_t i = emitters.size(); i > 0; --i) {
            const std::size_t idx = i - 1;
            emitters[idx]->update(proxy, dt, realDt);
            if (proxy.erased) {
                if (i != emitters.size()) { emitters[idx] = std::move(emitters.back()); }
                emitters.pop_back();
            }
            proxy.reset();
        }
    }

    // remove particles that did not get re-emitted
    for (std::size_t i : freeList) {
        if (particles.size() > 1 && i < particles.size() - 1) { particles[i] = particles.back(); }
        particles.pop_back();
    }
    freeList.clear();

    // run affectors over all particles
    if (!affectors.empty()) {
        std::vector<std::uint8_t> erased;
        erased.resize(affectors.size(), 0);

        futures.reserve(particles.size() / ParticlesPerThread + 1);
        auto it = particles.begin();
        while (it != particles.end()) {
            const std::size_t len =
                std::min<std::size_t>(std::distance(it, particles.end()), ParticlesPerThread);
            futures.emplace_back(threadPool.queueTask([this, it, len, dt, realDt, &erased]() {
                typename TAffector::Proxy proxy(*this, std::span<T>(&*it, len));
                unsigned int i = 0;
                for (auto& affector : affectors) {
                    affector->update(proxy, dt, realDt);
                    erased[i] = proxy.erased ? 1 : 0;
                    proxy.reset();
                    ++i;
                }
            }));
            it += len;
        }

        for (auto& f : futures) { f.wait(); }
        futures.clear();

        for (std::size_t i = affectors.size(); i > 0; --i) {
            const std::size_t idx = i - 1;
            if (erased[idx] != 0) {
                if (i != affectors.size()) { affectors[idx] = std::move(affectors.back()); }
                affectors.pop_back();
            }
        }
    }

    // update renderer data
    renderer.notifyData(particles.data(), particles.size());
}

template<typename T>
void ParticleManager<T>::removeUpdater(TUpdater* updater) {
    std::unique_lock lock(mutex);

    for (auto it = updaters.begin(); it != updaters.end(); ++it) {
        if (&*it == updater) {
            updaters.erase(it);
            return;
        }
    }
}

template<typename T>
void ParticleManager<T>::removeAllUpdaters() {
    std::unique_lock lock(mutex);
    updaters.clear();
}

template<typename T>
void ParticleManager<T>::removeAffector(TAffector* affector) {
    std::unique_lock lock(mutex);

    for (auto it = affectors.begin(); it != affectors.end(); ++it) {
        if (&*it == affector) {
            affectors.erase(it);
            return;
        }
    }
}

template<typename T>
void ParticleManager<T>::removeAllAffectors() {
    std::unique_lock lock(mutex);
    affectors.clear();
}

template<typename T>
void ParticleManager<T>::removeEmitter(TEmitter* emitter) {
    std::unique_lock lock(mutex);

    for (auto it = emitters.begin(); it != emitters.end(); ++it) {
        if (&*it == emitter) {
            emitters.erase(it);
            return;
        }
    }
}

template<typename T>
void ParticleManager<T>::removeAllEmitters() {
    std::unique_lock lock(mutex);
    emitters.clear();
}

template<typename T>
void ParticleManager<T>::removeSink(TSink* sink) {
    std::unique_lock lock(mutex);

    for (auto it = sinks.begin(); it != sinks.end(); ++it) {
        if (&*it == sink) {
            sinks.erase(it);
            return;
        }
    }
}

template<typename T>
void ParticleManager<T>::removeAllSinks() {
    std::unique_lock lock(mutex);
    sinks.clear();
}

template<typename T>
void ParticleManager<T>::clearAndReset() {
    std::unique_lock lock(mutex);

    affectors.clear();
    emitters.clear();
    sinks.clear();
    particles.clear();
    freed.clear();
    freeList.clear();
}

template<typename T>
void ParticleManager<T>::addToScene(rc::Scene* scene) {
    renderer.addToScene(scene);
}

template<typename T>
void ParticleManager<T>::removeFromScene() {
    renderer.removeFromScene();
}

template<typename T>
typename ParticleManager<T>::TRenderer& ParticleManager<T>::getRenderer() {
    return renderer;
}

template<typename T>
const typename ParticleManager<T>::TRenderer& ParticleManager<T>::getRenderer() const {
    return renderer;
}

template<typename T>
std::size_t ParticleManager<T>::getParticleCount() const {
    return particles.size();
}

template<typename T>
std::size_t ParticleManager<T>::getParticleCountLocked() const {
    std::unique_lock lock(mutex);
    return particles.size();
}

template<typename T>
void ParticleManager<T>::updateSink(std::size_t i, util::ThreadPool& pool, float dt, float realDt) {
    auto* sink = sinks[i].get();
    typename TSink::Proxy proxy(releaseMutex, *this, &particles.front(), freeList, freed);
    auto it = particles.begin();

    futures.reserve(particles.size() / ParticlesPerThread + 1);
    while (it != particles.end()) {
        const std::size_t len =
            std::min<std::size_t>(std::distance(it, particles.end()), ParticlesPerThread);
        futures.emplace_back(pool.queueTask([this, &proxy, it, len, sink, dt, realDt]() {
            sink->update(proxy, std::span<T>(&*it, len), dt, realDt);
        }));
        it += len;
    }

    for (auto& f : futures) { f.wait(); }
    futures.clear();

    if (proxy.erased) {
        if (i != sinks.size() - 1) { sinks[i] = std::move(sinks.back()); }
        sinks.pop_back();
    }
}

template<typename T>
template<typename U, typename... TArgs>
U* ParticleManager<T>::addUpdater(TArgs... args) {
    static_assert(std::is_base_of_v<TUpdater, U>, "U must derive from MetaUpdater");
    std::unique_lock lock(mutex);

    U* updater = new U(std::forward<TArgs>(args)...);
    updaters.emplace_back(updater);
    return updater;
}

template<typename T>
template<typename U>
U* ParticleManager<T>::getUpdater() {
    static_assert(std::is_base_of_v<TUpdater, U>, "U must derive from MetaUpdater");
    std::unique_lock lock(mutex);

    for (auto& a : updaters) {
        U* u = dynamic_cast<U*>(a.get());
        if (u) { return u; }
    }
    return nullptr;
}

template<typename T>
template<typename U>
void ParticleManager<T>::removeUpdaters() {
    static_assert(std::is_base_of_v<TUpdater, U>, "U must derive from MetaUpdater");
    std::unique_lock lock(mutex);

    std::erase_if(updaters, [](std::unique_ptr<TUpdater>& a) {
        return dynamic_cast<U*>(a.get()) != nullptr;
    });
}

template<typename T>
template<typename U, typename... TArgs>
U* ParticleManager<T>::addAffector(TArgs&&... args) {
    static_assert(std::is_base_of_v<TAffector, U>, "U must derive from Affector");
    std::unique_lock lock(mutex);

    U* affector = new U(std::forward<TArgs>(args)...);
    affectors.emplace_back(affector);
    return affector;
}

template<typename T>
template<typename U>
U* ParticleManager<T>::getAffector() {
    static_assert(std::is_base_of_v<TAffector, U>, "U must derive from Affector");
    std::unique_lock lock(mutex);

    for (auto& a : affectors) {
        U* u = dynamic_cast<U*>(a.get());
        if (u) { return u; }
    }
    return nullptr;
}

template<typename T>
template<typename U>
void ParticleManager<T>::removeAffectors() {
    static_assert(std::is_base_of_v<TAffector, U>, "U must derive from Affector");
    std::unique_lock lock(mutex);

    std::erase_if(affectors, [](std::unique_ptr<TAffector>& a) {
        return dynamic_cast<U*>(a.get()) != nullptr;
    });
}

template<typename T>
template<typename U, typename... TArgs>
U* ParticleManager<T>::addEmitter(TArgs&&... args) {
    static_assert(std::is_base_of_v<TEmitter, U>, "U must derive from Emitter");
    std::unique_lock lock(mutex);

    U* emitter = new U(std::forward<TArgs>(args)...);
    emitters.emplace_back(emitter);
    return emitter;
}

template<typename T>
template<typename U>
U* ParticleManager<T>::getEmitter() {
    static_assert(std::is_base_of_v<TEmitter, U>, "U must derive from Emitter");
    std::unique_lock lock(mutex);

    for (auto& e : emitters) {
        U* u = dynamic_cast<U*>(e.get());
        if (u) { return u; }
    }
    return nullptr;
}

template<typename T>
template<typename U>
void ParticleManager<T>::removeEmitters() {
    static_assert(std::is_base_of_v<TEmitter, U>, "U must derive from Emitter");
    std::unique_lock lock(mutex);

    std::erase_if(emitters, [](std::unique_ptr<TEmitter>& e) {
        return dynamic_cast<U*>(e.get()) != nullptr;
    });
}

template<typename T>
template<typename U, typename... TArgs>
U* ParticleManager<T>::addSink(TArgs&&... args) {
    static_assert(std::is_base_of_v<TSink, U>, "U must derive from Sink");
    std::unique_lock lock(mutex);

    U* sink = new U(std::forward<TArgs>(args)...);
    sinks.emplace_back(sink);
    return sink;
}

template<typename T>
template<typename U>
U* ParticleManager<T>::getSink() {
    static_assert(std::is_base_of_v<TSink, U>, "U must derive from Sink");
    std::unique_lock lock(mutex);

    for (auto& s : sinks) {
        U* u = dynamic_cast<U*>(s.get());
        if (u) { return u; }
    }
    return nullptr;
}

template<typename T>
template<typename U>
void ParticleManager<T>::removeSinks() {
    static_assert(std::is_base_of_v<TSink, U>, "U must derive from Sink");
    std::unique_lock lock(mutex);

    std::erase_if(sinks,
                  [](std::unique_ptr<TSink>& s) { return dynamic_cast<U*>(s.get()) != nullptr; });
}

} // namespace pcl
} // namespace bl

#endif
