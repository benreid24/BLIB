#ifndef BLIB_RENDER_DESCRIPTORS_SCENE3DUNLITINSTANCE_HPP
#define BLIB_RENDER_DESCRIPTORS_SCENE3DUNLITINSTANCE_HPP

#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Descriptors/SceneDescriptorSetInstance.hpp>
#include <BLIB/Render/Transfers/UniformBuffer.hpp>
#include <BLIB/Render/Vulkan/DescriptorPool.hpp>
#include <BLIB/Render/Vulkan/PerFrame.hpp>
#include <BLIB/Render/Vulkan/PerFrameVector.hpp>
#include <array>
#include <glm/glm.hpp>

namespace bl
{
namespace gfx
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
class Scene3DUnlitInstance : public SceneDescriptorSetInstance {
public:
    /**
     * @brief Creates a new instance of the descriptor set
     *
     * @param vulkanState Renderer Vulkan state
     * @param layout The layout of the descriptor set
     */
    Scene3DUnlitInstance(vk::VulkanState& vulkanState, VkDescriptorSetLayout layout);

    /**
     * @brief Destroys the descriptor set
     */
    virtual ~Scene3DUnlitInstance();

private:
    vk::VulkanState& vulkanState;
    const VkDescriptorSetLayout setLayout;
    vk::PerFrameVector<VkDescriptorSet> descriptorSets;
    vk::DescriptorPool::AllocationHandle allocHandle;

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
