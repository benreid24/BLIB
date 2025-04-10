#ifndef BLIB_RENDER_DESCRIPTORS_BUILTIN_COLORATTACHMENTINSTANCE_HPP
#define BLIB_RENDER_DESCRIPTORS_BUILTIN_COLORATTACHMENTINSTANCE_HPP

#include <BLIB/Render/Descriptors/DescriptorSetInstance.hpp>
#include <BLIB/Render/Vulkan/Framebuffer.hpp>
#include <BLIB/Render/Vulkan/PerFrame.hpp>

namespace bl
{
namespace rc
{
namespace ds
{
/**
 * @brief Descriptor set instance that provides a single binding for a color attachment
 *
 * @ingroup Renderer
 */
class ColorAttachmentInstance : public DescriptorSetInstance {
public:
    /**
     * @brief Creates the descriptor set instance
     *
     * @param vulkanState The renderer Vulkan state
     * @param layout The descriptor set layout
     */
    ColorAttachmentInstance(vk::VulkanState& vulkanState, VkDescriptorSetLayout layout);

    /**
     * @brief Destroys the descriptor set
     */
    virtual ~ColorAttachmentInstance();

    /**
     * @brief Must be called before rendering with the framebuffers to get input attachments from
     *
     * @param framebuffers The framebuffers. Must be Config::MaxConcurrentFrames buffers
     * @param attachmentIndex The index of the color attachment to bind
     * @param sampler The sampler to use
     */
    void initAttachments(const vk::Framebuffer* framebuffers, std::uint32_t attachmentIndex,
                         VkSampler sampler);

    /**
     * @brief Updates the descriptor for the current frame
     *
     * @param framebuffer The framebuffer to get the attachment from
     * @param attachmentIndex The index of the attachment to bind
     * @param sampler The sampler to use
     */
    void updateAttachment(const vk::Framebuffer& framebuffer, std::uint32_t attachmentIndex,
                          VkSampler sampler);

private:
    vk::VulkanState& vulkanState;
    VkDescriptorSetLayout layout;
    vk::DescriptorPool::AllocationHandle dsAlloc;
    vk::PerFrame<VkDescriptorSet> descriptorSets;

    virtual void bindForPipeline(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                                 std::uint32_t setIndex, UpdateSpeed updateFreq) const override;
    virtual void bindForObject(scene::SceneRenderContext&, VkPipelineLayout, std::uint32_t,
                               scene::Key) const override {}
    virtual void releaseObject(ecs::Entity, scene::Key) override {}
    virtual void init(DescriptorComponentStorageCache&) override {}
    virtual bool allocateObject(ecs::Entity, scene::Key) override { return true; }
    virtual void handleFrameStart() override {}
};

} // namespace ds
} // namespace rc
} // namespace bl

#endif
