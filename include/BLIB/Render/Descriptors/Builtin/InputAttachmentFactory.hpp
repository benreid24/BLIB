#ifndef BLIB_RENDER_DESCRIPTORS_INPUTATTACHMENTFACTORY_HPP
#define BLIB_RENDER_DESCRIPTORS_INPUTATTACHMENTFACTORY_HPP

#include <BLIB/Render/Descriptors/Builtin/InputAttachmentInstance.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetFactory.hpp>

#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace dsi
{
/**
 * @brief Descriptor set factory that provides a single binding for an attachment set
 *
 * @tparam AttachmentCount The number of attachments that will be bound
 * @tparam StartIndex The index of the first attachment to bind
 * @tparam ShaderStages The shader stages that will access the attachments
 * @ingroup Renderer
 */
template<std::uint32_t AttachmentCount, std::uint32_t StartIndex = 0,
         VkShaderStageFlags ShaderStages = VK_SHADER_STAGE_FRAGMENT_BIT>
class InputAttachmentFactory : public ds::DescriptorSetFactory {
public:
    /**
     * @brief Creates the factory
     */
    InputAttachmentFactory() = default;

    /**
     * @brief Frees resources
     */
    virtual ~InputAttachmentFactory() = default;

    /**
     * @brief Creates a descriptor set instance
     */
    virtual std::unique_ptr<ds::DescriptorSetInstance> createDescriptorSet() const override {
        return std::make_unique<InputAttachmentInstance>(
            *vs, descriptorSetLayout, AttachmentCount, StartIndex);
    }

private:
    vk::VulkanLayer* vs;

    virtual void init(engine::Engine&, Renderer& renderer) override {
        vs = &renderer.vulkanState();

        vk::DescriptorPool::SetBindingInfo bindingInfo;

        for (std::uint32_t i = 0; i < AttachmentCount; ++i) {
            bindingInfo.bindings[i].binding            = i;
            bindingInfo.bindings[i].descriptorCount    = 1;
            bindingInfo.bindings[i].descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            bindingInfo.bindings[i].pImmutableSamplers = nullptr;
            bindingInfo.bindings[i].stageFlags         = ShaderStages;
        }

        bindingInfo.bindingCount = AttachmentCount;
        descriptorSetLayout = renderer.vulkanState().getDescriptorPool().createLayout(bindingInfo);
    }

    virtual std::type_index creates() const override { return typeid(InputAttachmentInstance); }

    virtual bool isAutoConstructable() const override { return true; }
};

} // namespace dsi
} // namespace rc
} // namespace bl

#endif