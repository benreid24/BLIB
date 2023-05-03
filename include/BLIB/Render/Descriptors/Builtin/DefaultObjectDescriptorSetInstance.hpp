#ifndef BLIB_RENDER_DESCRIPTORS_BUILTIN_DEFAULTOBJECTDESCRIPTORSETINSTANCE_HPP
#define BLIB_RENDER_DESCRIPTORS_BUILTIN_DEFAULTOBJECTDESCRIPTORSETINSTANCE_HPP

#include <BLIB/ECS/Registry.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetInstance.hpp>
#include <BLIB/Render/Transfers/GenericBuffer.hpp>
#include <cstdint>
#include <glm/glm.hpp>
#include <vector>

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
 * @brief Descriptor set instance used by all objects in the engine default pipelines. Contains the
 *        object transform matrix and texture id
 *
 * @ingroup Renderer
 */
class DefaultObjectDescriptorSetInstance : public DescriptorSetInstance {
public:
    /**
     * @brief Create a new set instance
     *
     * @param engine Game engine instance
     * @param descriptorSetLayout Layout of the descriptor set
     */
    DefaultObjectDescriptorSetInstance(engine::Engine& engine,
                                       VkDescriptorSetLayout descriptorSetLayout);

    /**
     * @brief Frees resources
     */
    virtual ~DefaultObjectDescriptorSetInstance();

private:
    ecs::Registry& registry;
    VulkanState& vulkanState;
    VkDescriptorPool descriptorPool;
    const VkDescriptorSetLayout descriptorSetLayout;
    std::vector<PerFrame<VkDescriptorSet>> descriptorSets;
    std::vector<glm::mat4> transforms;
    std::vector<std::uint32_t> textures;
    tfr::GenericBuffer<glm::mat4, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, true> transformBuffer;
    tfr::GenericBuffer<std::uint32_t, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, true> textureBuffer;
    std::uint32_t staticObjectCount;
    std::uint32_t dynamicObjectCount;

    virtual void bindForPipeline(VkCommandBuffer commandBuffer, VkPipelineLayout layout,
                                 std::uint32_t observerIndex,
                                 std::uint32_t setIndex) const override;
    virtual void bindForObject(VkCommandBuffer commandBuffer, VkPipelineLayout layout,
                               std::uint32_t setIndex, std::uint32_t objectId) const override;
    virtual void releaseObject(std::uint32_t sceneId, ecs::Entity entity) override;
    virtual void doInit(std::uint32_t maxStaticObjects, std::uint32_t maxDynamicObjects) override;
    virtual bool doAllocateObject(std::uint32_t sceneId, ecs::Entity entity,
                                  SceneObject::UpdateSpeed updateSpeed) override;
    virtual void beginSync(bool staticObjectsChanged) override;
};

} // namespace ds
} // namespace render
} // namespace bl

#endif
