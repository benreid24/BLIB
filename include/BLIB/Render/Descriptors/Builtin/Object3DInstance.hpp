#ifndef BLIB_RENDER_DESCRIPTORS_BUILTIN_OBJECT3DINSTANCE_HPP
#define BLIB_RENDER_DESCRIPTORS_BUILTIN_OBJECT3DINSTANCE_HPP

#include <BLIB/ECS/Registry.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetInstance.hpp>
#include <BLIB/Render/Transfers/UniformBuffer.hpp>
#include <BLIB/Render/Transfers/UniformBufferDouble.hpp>
#include <BLIB/Render/Vulkan/PerFrameVector.hpp>
#include <cstdint>
#include <glm/glm.hpp>
#include <vector>

namespace bl
{
namespace engine
{
class Engine;
}

namespace gfx
{
class Renderer;

namespace ds
{
/**
 * @brief Descriptor set instance used by all meshes in the engine default pipelines. Contains the
 *        object transform matrix and texture id
 *
 * @ingroup Renderer
 */
class Object3DInstance : public DescriptorSetInstance {
public:
    /**
     * @brief Create a new set instance
     *
     * @param engine Game engine instance
     * @param descriptorSetLayout Layout of the descriptor set
     */
    Object3DInstance(engine::Engine& engine, VkDescriptorSetLayout descriptorSetLayout);

    /**
     * @brief Frees resources
     */
    virtual ~Object3DInstance();

private:
    ecs::Registry& registry;
    vk::VulkanState& vulkanState;
    vk::DescriptorPool::AllocationHandle alloc;
    const VkDescriptorSetLayout descriptorSetLayout;
    vk::PerFrameVector<VkDescriptorSet> descriptorSets;
    tfr::UniformBuffer<glm::mat4> transformBuffer;
    tfr::UniformBufferDouble<std::uint32_t> textureBuffer;
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
                                  UpdateSpeed updateSpeed) override;
    virtual void beginSync(bool staticObjectsChanged) override;
};

} // namespace ds
} // namespace gfx
} // namespace bl

#endif
