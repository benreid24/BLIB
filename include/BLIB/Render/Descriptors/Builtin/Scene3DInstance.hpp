#ifndef BLIB_RENDER_DESCRIPTORS_SCENE3DINSTANCE_HPP
#define BLIB_RENDER_DESCRIPTORS_SCENE3DINSTANCE_HPP

#include <BLIB/Render/Buffers/StaticUniformBuffer.hpp>
#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Descriptors/SceneDescriptorSetInstance.hpp>
#include <BLIB/Render/Lighting/LightingDescriptor3D.hpp>
#include <BLIB/Render/Lighting/PointLight3D.hpp>
#include <BLIB/Render/Lighting/SpotLight3D.hpp>
#include <BLIB/Render/Vulkan/DescriptorPool.hpp>
#include <BLIB/Render/Vulkan/PerFrame.hpp>
#include <BLIB/Render/Vulkan/PerFrameVector.hpp>
#include <array>
#include <glm/glm.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
struct VulkanState;
}

namespace ds
{
/**
 * @brief Descriptor set instance for common scene data
 *
 * @ingroup Renderer
 */
class Scene3DInstance : public SceneDescriptorSetInstance {
public:
    /**
     * @brief Creates a new instance of the descriptor set
     *
     * @param vulkanState Renderer Vulkan state
     * @param layout The layout of the descriptor set
     */
    Scene3DInstance(vk::VulkanState& vulkanState, VkDescriptorSetLayout layout);

    /**
     * @brief Destroys the descriptor set
     */
    virtual ~Scene3DInstance();

private:
    vk::VulkanState& vulkanState;
    const VkDescriptorSetLayout setLayout;
    vk::PerFrameVector<VkDescriptorSet> descriptorSets;
    vk::DescriptorPool::AllocationHandle allocHandle;
    buf::StaticUniformBuffer<lgt::LightingDescriptor3D> globalLightInfo;
    buf::StaticUniformBuffer<lgt::SpotLight3D> spotlights;
    buf::StaticUniformBuffer<lgt::PointLight3D> pointLights;

    virtual void bindForPipeline(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                                 std::uint32_t setIndex, UpdateSpeed updateFreq) const override;
    virtual void bindForObject(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                               std::uint32_t setIndex, scene::Key objectKey) const override;
    virtual void releaseObject(ecs::Entity entity, scene::Key objectKey) override;
    virtual void init(DescriptorComponentStorageCache& storageCache) override;
    virtual bool allocateObject(ecs::Entity entity, scene::Key key) override;
    virtual void handleFrameStart() override;
};

} // namespace ds
} // namespace rc
} // namespace bl

#endif
