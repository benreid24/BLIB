#ifndef BLIB_RENDER_DESCRIPTORS_SCENE3DINSTANCE_HPP
#define BLIB_RENDER_DESCRIPTORS_SCENE3DINSTANCE_HPP

#include <BLIB/Render/Buffers/StaticUniformBuffer.hpp>
#include <BLIB/Render/Descriptors/Builtin/CommonShaderInputs.hpp>
#include <BLIB/Render/Descriptors/SceneDescriptorSetInstance.hpp>
#include <BLIB/Render/Events/GraphEvents.hpp>
#include <BLIB/Render/Events/ShadowMapsInvalidated.hpp>
#include <BLIB/Render/Graph/Assets/SSAOAsset.hpp>
#include <BLIB/Render/Lighting/LightingDescriptor3D.hpp>
#include <BLIB/Render/Lighting/PointLight3D.hpp>
#include <BLIB/Render/Lighting/SpotLight3D.hpp>
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
namespace scene
{
class Scene3D;
}
namespace rgi
{
class ShadowMapAsset;
} // namespace rgi

namespace ds
{
/**
 * @brief Descriptor set instance for common scene data
 *
 * @ingroup Renderer
 */
class Scene3DInstance
: public SceneDescriptorSetInstance
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
     * @param pipelineLayout The current pipeline layout
     * @param bindIndex The set index to bind to
     * @param observerIndex The index of the current observer to bind for
     */
    void bind(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout,
              std::uint32_t bindIndex, std::uint32_t observerIndex);

    /**
     * @brief Returns the lighting uniform value
     */
    lgt::LightingDescriptor3D& getUniform() { return uniform[0]; }

private:
    scene::Scene3D* owner;
    Renderer& renderer;
    buf::StaticUniformBuffer<lgt::LightingDescriptor3D> uniform;
    vk::Image emptySpotShadowMap;
    vk::Image emptyPointShadowMap;
    vk::Image emptySSAOImage;
    ShadowMapCameraShaderInput* shadowMapCameras;
    rgi::ShadowMapAsset* shadowMaps;
    rgi::SSAOAsset* ssaoBuffer;

    virtual void bindForPipeline(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                                 std::uint32_t setIndex, UpdateSpeed updateFreq) const override;
    virtual void bindForObject(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                               std::uint32_t setIndex, scene::Key objectKey) const override;
    virtual void releaseObject(ecs::Entity entity, scene::Key objectKey) override;
    virtual void init(ShaderInputStore& storageCache) override;
    virtual bool allocateObject(ecs::Entity entity, scene::Key key) override;
    virtual void handleFrameStart() override;

    virtual void process(const event::SceneGraphAssetInitialized& event) override;
    virtual void process(const event::ShadowMapsInvalidated& event) override;
    void updateImageDescriptors();

    friend class scene::Scene3D;
};

} // namespace ds
} // namespace rc
} // namespace bl

#endif
