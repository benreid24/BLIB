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
namespace render
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
     * @brief Returns a pointer to the correct instance of the factory to use for the given derived
     *        type. Takes ownership and initializes the passed in factory if it is the first copy
     *
     * @param derivedType The typeid of the derived factory type being requested
     * @param factory An instance of the factory. May or may not take ownership
     * @return A pointer to the instance of the factory to use
     */
    DescriptorSetFactory* getFactory(std::type_index derivedType,
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

private:
    engine::Engine& engine;
    Renderer& renderer;
    std::unordered_map<std::type_index, std::unique_ptr<DescriptorSetFactory>> cache;

    DescriptorSetFactoryCache(engine::Engine& engine, Renderer& renderer);
    void cleanup();

    friend class Renderer;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T, typename... TArgs>
T* DescriptorSetFactoryCache::getOrCreateFactory(TArgs&&... args) {
    auto it = cache.find(typeid(T));
    if (it == cache.end()) {
        it = cache.try_emplace(typeid(T), std::make_unique<T>(std::forward<TArgs>(args)...)).first;
    }
    return static_cast<T*>(it->second.get());
}

} // namespace ds
} // namespace render
} // namespace bl

#endif
