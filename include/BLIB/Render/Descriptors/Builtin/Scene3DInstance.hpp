#ifndef BLIB_RENDER_DESCRIPTORS_SCENE3DINSTANCE_HPP
#define BLIB_RENDER_DESCRIPTORS_SCENE3DINSTANCE_HPP

#include <BLIB/Events.hpp>
#include <BLIB/Render/Buffers/StaticUniformBuffer.hpp>
#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Descriptors/Builtin/CommonShaderInputs.hpp>
#include <BLIB/Render/Descriptors/SceneDescriptorSetInstance.hpp>
#include <BLIB/Render/Events/GraphEvents.hpp>
#include <BLIB/Render/Lighting/LightingDescriptor3D.hpp>
#include <BLIB/Render/Lighting/PointLight3D.hpp>
#include <BLIB/Render/Lighting/SpotLight3D.hpp>
#include <BLIB/Render/Vulkan/DescriptorPool.hpp>
#include <BLIB/Render/Vulkan/PerFrame.hpp>
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

namespace ds
{
/**
 * @brief Descriptor set instance for common scene data
 *
 * @ingroup Renderer
 */
class Scene3DInstance
: public SceneDescriptorSetInstance
, public bl::event::Listener<event::SceneGraphAssetInitialized> {
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
     * @brief Returns the lighting uniform value
     */
    lgt::LightingDescriptor3D& getUniform() { return uniform[0]; }

private:
    scene::Scene3D* owner;
    Renderer& renderer;
    buf::StaticUniformBuffer<lgt::LightingDescriptor3D> uniform;
    vk::Image emptySpotShadowMap;
    vk::Image emptyPointShadowMap;
    ShadowMapCameraShaderInput* shadowMapCameras;

    virtual void bindForPipeline(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                                 std::uint32_t setIndex, UpdateSpeed updateFreq) const override;
    virtual void bindForObject(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                               std::uint32_t setIndex, scene::Key objectKey) const override;
    virtual void releaseObject(ecs::Entity entity, scene::Key objectKey) override;
    virtual void init(ShaderInputStore& storageCache) override;
    virtual bool allocateObject(ecs::Entity entity, scene::Key key) override;
    virtual void handleFrameStart() override;

    virtual void observe(const event::SceneGraphAssetInitialized& event) override;
    void updateShadowDescriptors(rg::GraphAsset* asset);

    friend class scene::Scene3D;
};

} // namespace ds
} // namespace rc
} // namespace bl

#endif
