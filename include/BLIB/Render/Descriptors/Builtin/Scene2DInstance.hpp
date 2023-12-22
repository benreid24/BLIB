#ifndef BLIB_RENDER_DESCRIPTORS_SCENE2DINSTANCE_HPP
#define BLIB_RENDER_DESCRIPTORS_SCENE2DINSTANCE_HPP

#include <BLIB/Render/Buffers/StaticUniformBuffer.hpp>
#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Descriptors/SceneDescriptorSetInstance.hpp>
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
 * @brief Descriptor set instance for 2d scene data
 *
 * @ingroup Renderer
 */
class Scene2DInstance : public SceneDescriptorSetInstance {
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

    // TODO - interface for updating lighting data

private:
    struct alignas(16) Light {
        glm::vec4 color;
        glm::vec2 position;
    };

    struct alignas(16) Lighting {
        std::uint32_t lightCount;
        glm::vec3 ambient;
        alignas(16) Light lights[1024];
    };

    vk::VulkanState& vulkanState;
    const VkDescriptorSetLayout setLayout;
    buf::StaticUniformBuffer<Lighting> lighting;
    vk::PerFrameVector<VkDescriptorSet> descriptorSets;
    vk::DescriptorPool::AllocationHandle allocHandle;

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
