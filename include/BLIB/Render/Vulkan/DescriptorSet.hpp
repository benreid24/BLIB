#ifndef BLIB_RENDER_VULKAN_DESCRIPTORSET_HPP
#define BLIB_RENDER_VULKAN_DESCRIPTORSET_HPP

#include <BLIB/Render/Vulkan/DescriptorPool.hpp>
#include <BLIB/Vulkan.hpp>

namespace bl
{
namespace rc
{
class Renderer;

namespace vk
{

/**
 * @brief Wrapper around a single allocated descriptor set from the DescriptorPool
 *
 * @ingroup Renderer
 */
class DescriptorSet {
public:
    /**
     * @brief Creates an un-allocated descriptor set
     */
    DescriptorSet();

    /**
     * @brief Creates an un-allocated descriptor set
     *
     * @param renderer Renderer Vulkan state
     */
    DescriptorSet(Renderer& renderer);

    /**
     * @brief Creates and allocates the descriptor set
     *
     * @param renderer The renderer instance
     * @param layout The layout to allocate
     * @param dedicatedPool True to use a dedicated descriptor pool, false to use shared pools
     */
    DescriptorSet(Renderer& renderer, VkDescriptorSetLayout layout, bool dedicatedPool = false);

    /**
     * @brief Deleted
     */
    DescriptorSet(const DescriptorSet&) = delete;

    /**
     * @brief Assumes ownership from the given descriptor set
     *
     * @param ds The set to take over. Is invalidated
     */
    DescriptorSet(DescriptorSet&& ds);

    /**
     * @brief Defers release of the set if allocated
     */
    ~DescriptorSet();

    /**
     * @brief Deleted
     */
    DescriptorSet& operator=(const DescriptorSet&) = delete;

    /**
     * @brief Creates and allocates the descriptor set
     *
     * @param vulkanState Renderer Vulkan state
     * @param layout The layout to allocate
     * @return A reference to this object
     */
    DescriptorSet& operator=(DescriptorSet&& ds);

    /**
     * @brief Deferred init of Vulkan state pointer. For use with PerFrame
     *
     * @param renderer The renderer instance
     */
    void init(Renderer& renderer);

    /**
     * @brief Creates and allocates the descriptor set. Defers release of the prior set if any
     *
     * @param vulkanState Renderer Vulkan state
     * @param layout The layout to allocate
     * @param dedicatedPool True to use a dedicated descriptor pool, false to use shared pools
     */
    void allocate(VkDescriptorSetLayout layout, bool dedicatedPool = false);

    /**
     * @brief Immediately releases the descriptor set if any
     */
    void release();

    /**
     * @brief Defers release of the descriptor set if any
     */
    void deferRelease();

    /**
     * @brief Returns the allocated descriptor set
     */
    VkDescriptorSet getSet() const { return set; }

    /**
     * @brief Issues the bind command into the command buffer to bind the descriptor set
     *
     * @param commandBuffer The command buffer to issue the command into
     * @param bindPoint The pipeline bind point to bind to
     * @param layout The pipeline layout of the currently bound pipeline
     * @param index The index to bind at
     */
    void bind(VkCommandBuffer commandBuffer, VkPipelineBindPoint bindPoint, VkPipelineLayout layout,
              std::uint32_t index);

private:
    Renderer* renderer;
    vk::DescriptorPool::AllocationHandle alloc;
    VkDescriptorSet set;
};

} // namespace vk
} // namespace rc
} // namespace bl

#endif
