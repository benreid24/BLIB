#ifndef BLIB_RENDER_DESCRIPTORS_DESCRIPTORSETFACTORY_HPP
#define BLIB_RENDER_DESCRIPTORS_DESCRIPTORSETFACTORY_HPP

#include <BLIB/Render/Descriptors/DescriptorSetInstance.hpp>
#include <BLIB/Vulkan.hpp>
#include <memory>

namespace bl
{
namespace engine
{
class Engine;
}

namespace rc
{
class Renderer;

/// Collection of classes and utilities for managing descriptor sets
namespace ds
{
/**
 * @brief Base class for descriptor set instances. Descriptor set factories are given to
 *        PipelineParameters and represent a descriptor set layout. When a scene needs a given
 *        descriptor set it will use the factory for the relevant pipeline to create the descriptor
 *        set instance. Factories exist in the pipeline cache, set instances exist per-scene.
 *        Pipelines that utilize the same factory type will use the same factory object
 *
 * @ingroup Renderer
 */
class DescriptorSetFactory {
public:
    /**
     * @brief Destroys the descriptor set factory
     */
    virtual ~DescriptorSetFactory() = default;

    /**
     * @brief This method will be called exactly once by the renderer the first time this descriptor
     *        set is created. Derived classes should put resource allocations in here instead of
     *        their constructors to avoid duplicating effort
     *
     * @param engine The game engine instance
     * @param renderer The renderer instance
     */
    virtual void init(engine::Engine& engine, Renderer& renderer) = 0;

    /**
     * @brief Returns the layout of the descriptor set that this factory creates
     */
    constexpr VkDescriptorSetLayout getDescriptorLayout() const;

    /**
     * @brief Called when a descriptor set instance of this type is needed for a scene
     *
     * @return A new instance of the descriptor set for this factory
     */
    virtual std::unique_ptr<DescriptorSetInstance> createDescriptorSet() const = 0;

    /**
     * @brief Should return the type of descriptor set that is created by this factory
     */
    virtual std::type_index creates() const = 0;

protected:
    VkDescriptorSetLayout descriptorSetLayout;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr VkDescriptorSetLayout DescriptorSetFactory::getDescriptorLayout() const {
    return descriptorSetLayout;
}

} // namespace ds
} // namespace rc
} // namespace bl

#endif
