#ifndef BLIB_PARTICLES_EMITTER_HPP
#define BLIB_PARTICLES_EMITTER_HPP

#include <vector>

namespace bl
{
namespace pcl
{
template<typename T>
class ParticleManager;

/**
 * @brief Base class for all particle emitters. Provides the interface used by ParticleManagers
 *
 * @tparam T The type of particle the emitter works with
 * @ingroup Particles
 */
template<typename T>
class Emitter {
public:
    /**
     * @brief Proxy interface used by emitters to create new particles
     */
    class Proxy {
    public:
        /**
         * @brief Creates a new particle with the given constructor arguments
         *
         * @tparam ...TArgs Argument types to the particle constructor
         * @param ...args Arguments to the particle constructor
         * @return A reference to the new particle. May be invalidated by future calls to emit()
         */
        template<typename... TArgs>
        T& emit(TArgs&&... args) {
            if (!freeList.empty()) {
                T* val = new (&storage[freeList.back()]) T(std::forward<TArgs>(args)...);
                freeList.pop_back();
                return *val;
            }
            else {
                storage.emplace_back(std::forward<TArgs>(args)...);
                return storage.back();
            }
        }

        /**
         * @brief Call if the emitter needs to remove itself from the particle system
         */
        void eraseMe() { erased = true; }

        /**
         * @brief Access the particle manager that owns this emitter
         */
        const ParticleManager<T>& getManager() const { return manager; }

    private:
        ParticleManager<T>& manager;
        std::vector<T>& storage;
        std::vector<std::size_t>& freeList;
        bool erased;

        Proxy(ParticleManager<T>& manager, std::vector<T>& storage,
              std::vector<std::size_t>& freeList)
        : manager(manager)
        , storage(storage)
        , freeList(freeList)
        , erased(false) {}

        void reset() { erased = false; }

        friend class ParticleManager<T>;
    };

    /**
     * @brief Destroys the emitter
     */
    virtual ~Emitter() = default;

    /**
     * @brief Called once per frame. Emitters should create new particles in here
     *
     * @param proxy The proxy to use to create new particles
     * @param dt Elapsed simulation time, in seconds, since last call to update
     * @param realDt Elapsed real time, in seconds, since last call to update
     */
    virtual void update(Proxy& proxy, float dt, float realDt) = 0;
};

} // namespace pcl
} // namespace bl

#endif
