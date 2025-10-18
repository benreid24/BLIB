#ifndef BLIB_RENDER_DESCRIPTORS_SCENE3DINSTANCE_HPP
#define BLIB_RENDER_DESCRIPTORS_SCENE3DINSTANCE_HPP

#include <BLIB/Render/Buffers/StaticUniformBuffer.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetInstance.hpp>
#include <BLIB/Render/Events/GraphEvents.hpp>
#include <BLIB/Render/Events/ShadowMapsInvalidated.hpp>
#include <BLIB/Render/Graph/Assets/SSAOAsset.hpp>
#include <BLIB/Render/Lighting/LightingDescriptor3D.hpp>
#include <BLIB/Render/Lighting/PointLight3D.hpp>
#include <BLIB/Render/Lighting/SpotLight3D.hpp>
#include <BLIB/Render/ShaderResources/CameraBufferShaderResource.hpp>
#include <BLIB/Render/ShaderResources/ShadowMapCameraShaderResource.hpp>
#include <BLIB/Render/Vulkan/DescriptorPool.hpp>
#include <BLIB/Render/Vulkan/PerFrame.hpp>
#include <BLIB/Signals/Listener.hpp>
#include <array>
#include <glm/glm.hpp>

namespace bl
{
namespace rc
{
class Renderer;
namespace vk
{
struct VulkanState;
}
namespace rgi
{
class ShadowMapAsset;
} // namespace rgi

namespace dsi
{
/**
 * @brief Descriptor set instance for common scene data
 *
 * @ingroup Renderer
 */
class Scene3DInstance
: public ds::DescriptorSetInstance
, public sig::Listener<event::SceneGraphAssetInitialized, event::ShadowMapsInvalidated> {
public:
    /**
     * @brief Creates a new instance of the descriptor set
     *
     * @param renderer The renderer instance
     * @param layout The layout of the descriptor set
     */
    Scene3DInstance(Renderer& renderer, VkDescriptorSetLayout layout);

    /**
     * @brief Destroys the descriptor set
     */
    virtual ~Scene3DInstance();

    /**
     * @brief Binds the descriptor set
     *
     * @param commandBuffer The command buffer to write the command into
     * @param pipelineBindPoint The bind point of the active pipeline
     * @param pipelineLayout The current pipeline layout
     * @param bindIndex The set index to bind to
     */
    void bind(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint,
              VkPipelineLayout pipelineLayout, std::uint32_t bindIndex);

private:
    Renderer& renderer;
    const VkDescriptorSetLayout setLayout;
    vk::DescriptorPool::AllocationHandle allocHandle;
    vk::PerFrame<VkDescriptorSet> descriptorSets;
    sri::CameraBufferShaderResource* cameraBuffer;
    sri::LightingBuffer3D* lightBuffer;
    vk::Image emptySpotShadowMap;
    vk::Image emptyPointShadowMap;
    vk::Image emptySSAOImage;
    sri::ShadowMapCameraShaderResource* shadowMapCameras;
    rgi::ShadowMapAsset* shadowMaps;
    rgi::SSAOAsset* ssaoBuffer;

    virtual void bindForPipeline(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                                 std::uint32_t setIndex, UpdateSpeed updateFreq) const override;
    virtual void bindForObject(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                               std::uint32_t setIndex, scene::Key objectKey) const override;
    virtual void releaseObject(ecs::Entity entity, scene::Key objectKey) override;
    virtual void init(ds::InitContext& ctx) override;
    virtual bool allocateObject(ecs::Entity entity, scene::Key key) override;
    virtual void updateDescriptors() override;

    virtual void process(const event::SceneGraphAssetInitialized& event) override;
    virtual void process(const event::ShadowMapsInvalidated& event) override;
    void updateImageDescriptors();
};

} // namespace dsi
} // namespace rc
} // namespace bl

#endif
