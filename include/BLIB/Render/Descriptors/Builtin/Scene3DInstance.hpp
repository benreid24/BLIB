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
namespace lgt
{
class Scene3DLighting;
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
     * @param renderer The renderer instance
     * @param layout The layout of the descriptor set
     */
    Scene3DInstance(Renderer& renderer, VkDescriptorSetLayout layout);

    /**
     * @brief Destroys the descriptor set
     */
    virtual ~Scene3DInstance();

    /**
     * @brief Returns the image for the spotlight shadow map
     *
     * @param i The index of the shadow map image to get
     * @return The shadow map at the given index
     */
    const vk::Image& getSpotlightShadowMap(unsigned int i) const;

    /**
     * @brief Returns the image for the point light shadow map
     *
     * @param i The index of the shadow map image to get
     * @return The shadow map at the given index
     */
    const vk::Image& getPointLightShadowMap(unsigned int i) const;

private:
    Renderer& renderer;
    buf::StaticUniformBuffer<lgt::LightingDescriptor3D> globalLightInfo;
    buf::StaticUniformBuffer<lgt::SpotLight3D> spotlights;
    buf::StaticUniformBuffer<lgt::PointLight3D> pointLights;
    buf::StaticUniformBuffer<lgt::SpotLight3D> spotlightsWithShadows;
    std::array<vk::Image, Config::MaxSpotShadows> spotShadowMapImages;
    buf::StaticUniformBuffer<lgt::PointLight3D> pointLightsWithShadows;
    std::array<vk::Image, Config::MaxPointShadows> pointShadowMapImages;

    virtual void bindForPipeline(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                                 std::uint32_t setIndex, UpdateSpeed updateFreq) const override;
    virtual void bindForObject(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                               std::uint32_t setIndex, scene::Key objectKey) const override;
    virtual void releaseObject(ecs::Entity entity, scene::Key objectKey) override;
    virtual void init(DescriptorComponentStorageCache& storageCache) override;
    virtual bool allocateObject(ecs::Entity entity, scene::Key key) override;
    virtual void handleFrameStart() override;

    friend class lgt::Scene3DLighting;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline const vk::Image& Scene3DInstance::getSpotlightShadowMap(unsigned int i) const {
    return spotShadowMapImages[i];
}

inline const vk::Image& Scene3DInstance::getPointLightShadowMap(unsigned int i) const {
    return pointShadowMapImages[i];
}

} // namespace ds
} // namespace rc
} // namespace bl

#endif
