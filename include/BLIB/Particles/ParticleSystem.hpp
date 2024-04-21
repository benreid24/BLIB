#ifndef BLIB_PARTICLES_PARTICLESYSTEM_HPP
#define BLIB_PARTICLES_PARTICLESYSTEM_HPP

#include <BLIB/Engine/System.hpp>
#include <BLIB/Particles/ParticleManager.hpp>
#include <typeindex>
#include <unordered_map>

namespace bl
{
/// The BLIB particle system
namespace pcl
{
/**
 * @brief Top level engine system for the BLIB particle system. Owns all the ParticleManagers for
 *        each particle type
 *
 * @ingroup Particles
 */
class ParticleSystem : public engine::System {
public:
    /**
     * @brief Initializes the particle system
     */
    ParticleSystem();

    /**
     * @brief Destroys the particle system
     */
    virtual ~ParticleSystem() = default;

    /**
     * @brief Returns a particle manager for the given particle type. Ensures only one manager is
     *        created. Use for global particle managers
     *
     * @tparam T The particle type to get or create the manager for
     * @return A unique particle manager for the given particle type
     */
    template<typename T>
    ParticleManager<T>& getUniqueSystem();

    /**
     * @brief Removes the unique particle manager for the given particle type
     *
     * @tparam T The particle type of the manager to remove
     */
    template<typename T>
    void removeUniqueSystem();

    /**
     * @brief Removes the given unique particle manager
     *
     * @param system The particle manager to remove
     */
    void removeUniqueSystem(ParticleManagerBase* system);

    /**
     * @brief Removes all unique particle managers
     */
    void removeUniqueSystems();

    /**
     * @brief Adds a new particle manager for the given particle type. Always creates a new one. Use
     *        for entity level particle managers
     *
     * @tparam T The particle type
     * @return The newly created particle manager
     */
    template<typename T>
    ParticleManager<T>& addRepeatedSystem();

    /**
     * @brief Removes the specific entity level particle manager
     *
     * @param system The particle manager to remove
     */
    void removeRepeatedSystem(ParticleManagerBase* system);

    /**
     * @brief Removes the specific entity level particle manager. Prefer this typed version to the
     *        generic for faster erasing
     *
     * @tparam T The particle type
     * @param system The particle manager to remove
     */
    template<typename T>
    void removeRepeatedSystem(ParticleManager<T>* system);

    /**
     * @brief Removes all particle managers of the given type
     *
     * @tparam T The particle type to remove all managers for
     */
    template<typename T>
    void removeRepeatedSystems();

    /**
     * @brief Removes all particle managers
     */
    void removeAllSystems();

private:
    engine::Engine* engine;
    std::mutex mutex;
    util::ThreadPool* engineThreadpool;
    util::ThreadPool particleThreadpool;
    std::unordered_map<std::type_index, std::unique_ptr<ParticleManagerBase>> singleSystems;
    std::unordered_map<std::type_index, std::vector<std::unique_ptr<ParticleManagerBase>>>
        multiSystems;
    std::vector<std::future<void>> futures;

    virtual void init(engine::Engine& engine) override;
    virtual void update(std::mutex& stageMutex, float dt, float realDt, float residual,
                        float realResidual) override;
    virtual void earlyCleanup() override;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
ParticleManager<T>& ParticleSystem::getUniqueSystem() {
    using U = ParticleManager<T>;

    std::unique_lock lock(mutex);

    auto it = singleSystems.find(typeid(T));
    if (it == singleSystems.end()) {
        it = singleSystems.try_emplace(typeid(T), std::make_unique<U>()).first;
        it->second->init(*engine);
    }
    return static_cast<U&>(*it->second);
}

template<typename T>
void ParticleSystem::removeUniqueSystem() {
    std::unique_lock lock(mutex);
    singleSystems.erase(typeid(T));
}

template<typename T>
ParticleManager<T>& ParticleSystem::addRepeatedSystem() {
    using U = ParticleManager<T>;

    std::unique_lock lock(mutex);

    auto& list  = multiSystems[typeid(T)];
    auto& added = list.emplace_back(std::make_unique<U>());
    added->init(*engine);
    return static_cast<U&>(*added);
}

template<typename T>
void ParticleSystem::removeRepeatedSystem(ParticleManager<T>* system) {
    using U = ParticleManager<T>;

    std::unique_lock lock(mutex);

    const auto it = multiSystems.find(typeid(T));
    if (it != multiSystems.end()) {
        for (auto mit = it->second.begin(); mit != it->second.end(); ++mit) {
            if (mit->get() == static_cast<ParticleManagerBase*>(system)) {
                it->second.erase(mit);
                return;
            }
        }
    }
}

template<typename T>
void ParticleSystem::removeRepeatedSystems() {
    std::unique_lock lock(mutex);
    multiSystems.erase(typeid(T));
}

} // namespace pcl
} // namespace bl

#endif
