#ifndef BLIB_RENDER_DESCRIPTORS_DESCRIPTORSETFACTORYCACHE_HPP
#define BLIB_RENDER_DESCRIPTORS_DESCRIPTORSETFACTORYCACHE_HPP

#include <BLIB/Render/Descriptors/DescriptorSetFactory.hpp>
#include <memory>
#include <typeindex>
#include <unordered_map>

namespace bl
{
namespace engine
{
class Engine;
}
namespace rc
{
class Renderer;

namespace ds
{
/**
 * @brief Internal cache for descriptor set factories. Ensures that only one instance of each
 *        derived factory type exists in the renderer and ensures that each factory type is
 *        initialized exactly once and cleaned up during renderer cleanup
 *
 * @ingroup Renderer
 */
class DescriptorSetFactoryCache {
public:
    /**
     * @brief Returns a pointer to a contained factory of the given type, or nullptr if not found
     *
     * @tparam T The factory type to fetch
     * @return Pointer to the contained factory or nullptr if not found
     */
    template<typename T>
    T* getFactory();

    /**
     * @brief Returns a pointer to the correct instance of the factory to use for the given derived
     *        type. Takes ownership and initializes the passed in factory if it is the first copy
     *
     * @param derivedType The typeid of the derived factory type being requested
     * @param factory An instance of the factory. May or may not take ownership
     * @return A pointer to the instance of the factory to use
     */
    DescriptorSetFactory* getOrAddFactory(std::type_index derivedType,
                                          std::unique_ptr<DescriptorSetFactory>&& factory);

    /**
     * @brief Helper method for fetching or creating a factory without unnecessarily allocating a
     *        new factory if one is already in the cache
     *
     * @tparam T Type of factory to create
     * @tparam ...TArgs Argument types to factory constructor
     * @param ...args Arguments to factory constructor
     * @return Pointer to a factory of type T in the cache
     */
    template<typename T, typename... TArgs>
    T* getOrCreateFactory(TArgs&&... args);

    /**
     * @brief Returns the factory that creates the given type
     *
     * @tparam T The descriptor set to search for
     * @return The factory that creates the given set type, or nullptr if not found
     */
    template<typename T>
    DescriptorSetFactory* getFactoryThatMakes();

private:
    engine::Engine& engine;
    Renderer& renderer;
    std::unordered_map<std::type_index, std::unique_ptr<DescriptorSetFactory>> cache;

    DescriptorSetFactoryCache(engine::Engine& engine, Renderer& renderer);
    void cleanup();

    friend class bl::rc::Renderer;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
inline T* DescriptorSetFactoryCache::getFactory() {
    auto it = cache.find(typeid(T));
    if (it == cache.end()) { return nullptr; }
    return static_cast<T*>(it->second.get());
}

template<typename T, typename... TArgs>
T* DescriptorSetFactoryCache::getOrCreateFactory(TArgs&&... args) {
    auto it = cache.find(typeid(T));
    if (it == cache.end()) {
        it = cache.try_emplace(typeid(T), std::make_unique<T>(std::forward<TArgs>(args)...)).first;
        it->second->init(engine, renderer);
    }
    return static_cast<T*>(it->second.get());
}

template<typename T>
DescriptorSetFactory* DescriptorSetFactoryCache::getFactoryThatMakes() {
    for (auto& p : cache) {
        if (p.second->creates() == typeid(T)) { return p.second.get(); }
    }
    return nullptr;
}

} // namespace ds
} // namespace rc
} // namespace bl

#endif
