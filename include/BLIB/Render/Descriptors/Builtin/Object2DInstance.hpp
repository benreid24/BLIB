#ifndef BLIB_RENDER_DESCRIPTORS_BUILTIN_OBJECT2DINSTANCE_HPP
#define BLIB_RENDER_DESCRIPTORS_BUILTIN_OBJECT2DINSTANCE_HPP

#include <BLIB/ECS/Registry.hpp>
#include <BLIB/Render/Buffers/StaticSSBO.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetInstance.hpp>
#include <BLIB/Render/Transfers/UniformBuffer.hpp>
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
 * @brief Descriptor set instance used by all 2d objects in the engine default pipelines. Contains
 *        the object transform matrix and texture id
 *
 * @ingroup Renderer
 */
class Object2DInstance : public DescriptorSetInstance {
public:
    /**
     * @brief Create a new set instance
     *
     * @param engine Game engine instance
     * @param descriptorSetLayout Layout of the descriptor set
     */
    Object2DInstance(engine::Engine& engine, VkDescriptorSetLayout descriptorSetLayout);

    /**
     * @brief Frees resources
     */
    virtual ~Object2DInstance();

private:
    ecs::Registry& registry;
    vk::VulkanState& vulkanState;
    vk::DescriptorPool::AllocationHandle alloc;
    const VkDescriptorSetLayout descriptorSetLayout;
    vk::PerFrame<VkDescriptorSet> descriptorSets;

    buf::StaticSSBO<glm::mat4> transformBuffer;
    buf::StaticSSBO<std::uint32_t> textureBuffer;
    // TODO - dynamic ssbo

    virtual void bindForPipeline(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                                 std::uint32_t setIndex) const override;
    virtual void bindForObject(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                               std::uint32_t setIndex, std::uint32_t objectId) const override;
    virtual void releaseObject(std::uint32_t sceneId, ecs::Entity entity) override;
    virtual void doInit(std::uint32_t maxStaticObjects, std::uint32_t maxDynamicObjects) override;
    virtual bool doAllocateObject(std::uint32_t sceneId, ecs::Entity entity,
                                  UpdateSpeed updateSpeed) override;
    virtual void beginSync(bool staticObjectsChanged) override;

    void updateStaticDescriptors();
};

} // namespace ds
} // namespace gfx
} // namespace bl

#endif
