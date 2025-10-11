#ifndef BLIB_RENDER_DESCRIPTORS_BUILTIN_INPUTATTACHMENTINSTANCE_HPP
#define BLIB_RENDER_DESCRIPTORS_BUILTIN_INPUTATTACHMENTINSTANCE_HPP

#include <BLIB/Render/Config/Limits.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetInstance.hpp>
#include <BLIB/Render/Vulkan/DescriptorPool.hpp>
#include <BLIB/Render/Vulkan/Framebuffer.hpp>
#include <BLIB/Render/Vulkan/PerFrame.hpp>

namespace bl
{
namespace rc
{
namespace dsi
{
/**
 * @brief Descriptor set instance that provides bindings for an entire attachment set
 *
 * @ingroup Renderer
 */
class InputAttachmentInstance : public ds::DescriptorSetInstance {
public:
    /**
     * @brief Creates the descriptor set instance
     *
     * @param vulkanState The renderer Vulkan state
     * @param layout The descriptor set layout
     * @param attachmentCount The number of attachments that will be bound
     * @param startIndex The index of the first attachment to bind
     */
    InputAttachmentInstance(vk::VulkanState& vulkanState, VkDescriptorSetLayout layout,
                            std::uint32_t attachmentCount, std::uint32_t startIndex);

    /**
     * @brief Destroys the descriptor set
     */
    virtual ~InputAttachmentInstance();

    /**
     * @brief Creates the descriptor set with the given attachments
     *
     * @param attachmentSet The attachment set to bind
     * @param sampler The sampler to use
     */
    void initAttachments(const vk::AttachmentSet* attachmentSet, VkSampler sampler);

    /**
     * @brief Creates the descriptor set with the given attachments
     *
     * @param attachmentSets The attachment sets to bind per frame
     * @param sampler The sampler to use
     */
    void initAttachments(const std::array<const vk::AttachmentSet*,
                                          cfg::Limits::MaxConcurrentFrames>& attachmentSets,
                         VkSampler sampler);

    /**
     * @brief Issues the command to bind the descriptor set, updating if the underlying images have
     *        changed
     *
     * @param commandBuffer The command buffer to write commands into
     * @param pipelineBindPoint The bind point of the active pipeline
     * @param layout The layout of the current pipeline
     * @param setIndex The index to bind the set at
     */
    void bind(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint,
              VkPipelineLayout layout, std::uint32_t setIndex) const;

private:
    class Source {
    public:
        Source() = default;

        void init(const vk::AttachmentSet* attachmentSet) {
            type   = Type::Single;
            single = attachmentSet;
        }

        void init(const std::array<const vk::AttachmentSet*, cfg::Limits::MaxConcurrentFrames>&
                      attachmentSets) {
            type     = Type::PerFrame;
            perFrame = attachmentSets;
        }

        const vk::AttachmentSet* get(std::uint32_t index) const {
            switch (type) {
            case Type::PerFrame:
                return perFrame[index];
            case Type::Single:
            default:
                return single;
            }
        }

    private:
        enum struct Type { Single, PerFrame };
        Type type;
        union {
            const vk::AttachmentSet* single;
            std::array<const vk::AttachmentSet*, cfg::Limits::MaxConcurrentFrames> perFrame;
        };
    };

    const std::uint32_t startIndex;
    const std::uint32_t attachmentCount;
    vk::VulkanState& vulkanState;
    VkDescriptorSetLayout layout;
    vk::DescriptorPool::AllocationHandle dsAlloc;
    vk::PerFrame<VkDescriptorSet> descriptorSets;
    Source source;
    mutable vk::PerFrame<std::array<VkImageView, 8>> cachedViews;
    VkSampler sampler;

    void commonInit();

    virtual void bindForPipeline(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                                 std::uint32_t setIndex, UpdateSpeed updateFreq) const override;
    virtual void bindForObject(scene::SceneRenderContext&, VkPipelineLayout, std::uint32_t,
                               scene::Key) const override {}
    virtual void releaseObject(ecs::Entity, scene::Key) override {}
    virtual void init(sr::ShaderResourceStore&) override {}
    virtual bool allocateObject(ecs::Entity, scene::Key) override { return true; }
    virtual void updateDescriptors() override {}
};

} // namespace dsi
} // namespace rc
} // namespace bl

#endif
