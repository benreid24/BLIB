#ifndef BLIB_RENDER_DESCRIPTORS_SCENE2DINSTANCE_HPP
#define BLIB_RENDER_DESCRIPTORS_SCENE2DINSTANCE_HPP

#include <BLIB/Render/Descriptors/DescriptorSetInstance.hpp>
#include <BLIB/Render/Lighting/Scene2DLighting.hpp>
#include <BLIB/Render/ShaderResources/CameraBufferShaderResource.hpp>
#include <BLIB/Render/Vulkan/DescriptorPool.hpp>
#include <BLIB/Render/Vulkan/PerFrame.hpp>
#include <BLIB/Render/Vulkan/PerFrameVector.hpp>
#include <array>
#include <glm/glm.hpp>

namespace bl
{
namespace rc
{
namespace lgt
{
class Scene2DLighting;
class Light2D;
} // namespace lgt

namespace vk
{
struct VulkanState;
}

namespace dsi
{
/**
 * @brief Descriptor set instance for 2d scene data
 *
 * @ingroup Renderer
 */
class Scene2DInstance : public ds::DescriptorSetInstance {
public:
    /**
     * @brief Creates a new instance of the descriptor set
     *
     * @param vulkanState Renderer Vulkan state
     * @param layout The layout of the descriptor set
     */
    Scene2DInstance(vk::VulkanState& vulkanState, VkDescriptorSetLayout layout);

    /**
     * @brief Destroys the descriptor set
     */
    virtual ~Scene2DInstance();

private:
    vk::VulkanState& vulkanState;
    const VkDescriptorSetLayout setLayout;
    vk::DescriptorPool::AllocationHandle allocHandle;
    vk::PerFrame<VkDescriptorSet> descriptorSets;
    sri::CameraBufferShaderResource* cameraBuffer;
    sri::LightingBuffer2D* lightingBuffer;

    virtual void bindForPipeline(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                                 std::uint32_t setIndex, UpdateSpeed updateFreq) const override;
    virtual void bindForObject(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                               std::uint32_t setIndex, scene::Key objectKey) const override;
    virtual void releaseObject(ecs::Entity entity, scene::Key objectKey) override;
    virtual void init(ds::InitContext& ctx) override;
    virtual bool allocateObject(ecs::Entity entity, scene::Key key) override;
    virtual void updateDescriptors() override;

    friend class lgt::Scene2DLighting;
    friend class lgt::Light2D;
};

} // namespace dsi
} // namespace rc
} // namespace bl

#endif
