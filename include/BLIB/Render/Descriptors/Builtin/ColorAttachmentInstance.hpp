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
     * @brief Creates the descriptor set with the given attachments
     *
     * @param attachmentSet The attachment set to bind
     * @param attachmentIndex The index of the color attachment to bind
     * @param sampler The sampler to use
     */
    void initAttachments(const vk::AttachmentSet* attachmentSet, std::uint32_t attachmentIndex,
                         VkSampler sampler);

    /**
     * @brief Creates the descriptor set with the given attachments
     *
     * @param attachmentSets The attachment sets to bind per frame
     * @param attachmentIndex The index of the color attachment to bind
     * @param sampler The sampler to use
     */
    void initAttachments(
        const std::array<const vk::AttachmentSet*, Config::MaxConcurrentFrames>& attachmentSets,
        std::uint32_t attachmentIndex, VkSampler sampler);

    /**
     * @brief Issues the command to bind the descriptor set, updating if the underlying images have
     *        changed
     *
     * @param commandBuffer The command buffer to write commands into
     * @param layout The layout of the current pipeline
     * @param setIndex The index to bind the set at
     */
    void bind(VkCommandBuffer commandBuffer, VkPipelineLayout layout, std::uint32_t setIndex) const;

private:
    class Source {
    public:
        Source() = default;

        void init(const vk::AttachmentSet* attachmentSet) {
            type   = Type::Single;
            single = attachmentSet;
        }

        void init(const std::array<const vk::AttachmentSet*, Config::MaxConcurrentFrames>&
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
            std::array<const vk::AttachmentSet*, Config::MaxConcurrentFrames> perFrame;
        };
    };

    vk::VulkanState& vulkanState;
    VkDescriptorSetLayout layout;
    vk::DescriptorPool::AllocationHandle dsAlloc;
    vk::PerFrame<VkDescriptorSet> descriptorSets;
    Source source;
    mutable vk::PerFrame<VkImageView> cachedViews;
    std::uint32_t attachmentIndex;
    VkSampler sampler;

    void commonInit();

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
