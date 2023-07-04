#ifndef BLIB_RENDER_DESCRIPTORS_SCENE2DINSTANCE_HPP
#define BLIB_RENDER_DESCRIPTORS_SCENE2DINSTANCE_HPP

#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Descriptors/SceneDescriptorSetInstance.hpp>
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

private:
    vk::VulkanState& vulkanState;
    const VkDescriptorSetLayout setLayout;
    vk::PerFrameVector<VkDescriptorSet> descriptorSets;
    vk::DescriptorPool::AllocationHandle allocHandle;

    virtual void bindForPipeline(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                                 std::uint32_t setIndex, UpdateSpeed updateFreq) const override;
    virtual void bindForObject(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                               std::uint32_t setIndex, scene::Key objectKey) const override;
    virtual void releaseObject(ecs::Entity entity, scene::Key objectKey) override;
    virtual void init() override;
    virtual bool doAllocateObject(ecs::Entity entity, scene::Key key) override;
    virtual void beginSync(DirtyRange dirtyStatic, DirtyRange dirtyDynamic) override;
};

} // namespace ds
} // namespace gfx
} // namespace bl

#endif
