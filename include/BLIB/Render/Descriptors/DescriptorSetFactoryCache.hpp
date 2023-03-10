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

private:
    engine::Engine& engine;
    Renderer& renderer;
    std::unordered_map<std::type_index, std::unique_ptr<DescriptorSetFactory>> cache;

    DescriptorSetFactoryCache(engine::Engine& engine, Renderer& renderer);
    void cleanup();

    friend class Renderer;
};

} // namespace ds
} // namespace render
} // namespace bl

#endif
