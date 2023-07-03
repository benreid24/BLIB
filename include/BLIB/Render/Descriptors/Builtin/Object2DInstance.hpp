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
    vk::PerFrame<VkDescriptorSet> staticDescriptorSets;
    vk::PerFrame<VkDescriptorSet> dynamicDescriptorSets;

    buf::StaticSSBO<glm::mat4> transformBufferStatic;
    buf::StaticSSBO<std::uint32_t> textureBufferStatic;
    // TODO - dynamic ssbo
    buf::StaticSSBO<glm::mat4> transformBufferDynamic;
    buf::StaticSSBO<std::uint32_t> textureBufferDynamic;

    virtual void bindForPipeline(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                                 std::uint32_t setIndex, UpdateSpeed updateFreq) const override;
    virtual void bindForObject(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                               std::uint32_t setIndex, scene::Key objectKey) const override;
    virtual void releaseObject(ecs::Entity entity, scene::Key objectKey) override;
    virtual void init() override;
    virtual bool doAllocateObject(ecs::Entity entity, scene::Key key) override;
    virtual void beginSync(DirtyRange dirtyStatic, DirtyRange dirtyDynamic) override;

    void updateStaticDescriptors();
    void updateDynamicDescriptors();
};

} // namespace ds
} // namespace gfx
} // namespace bl

#endif
