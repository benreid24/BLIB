#ifndef BLIB_RENDER_DESCRIPTORS_INPUTATTACHMENTFACTORY_HPP
#define BLIB_RENDER_DESCRIPTORS_INPUTATTACHMENTFACTORY_HPP

#include <BLIB/Render/Descriptors/Builtin/InputAttachmentInstance.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetFactory.hpp>

#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace ds
{
/**
 * @brief Descriptor set factory that provides a single binding for an attachment set
 *
 * @tparam AttachmentCount The number of attachments that will be bound
 * @tparam StartIndex The index of the first attachment to bind
 * @ingroup Renderer
 */
template<std::uint32_t AttachmentCount, std::uint32_t StartIndex = 0>
class InputAttachmentFactory : public DescriptorSetFactory {
public:
    /**
     * @brief Creates the factory
     */
    InputAttachmentFactory() = default;

    /**
     * @brief Frees resources
     */
    virtual ~InputAttachmentFactory() = default;

private:
    vk::VulkanState* vs;

    virtual void init(engine::Engine&, Renderer& renderer) override {
        vs = &renderer.vulkanState();

        vk::DescriptorPool::SetBindingInfo bindingInfo;

        for (std::uint32_t i = 0; i < AttachmentCount; ++i) {
            bindingInfo.bindings[i].binding            = i;
            bindingInfo.bindings[i].descriptorCount    = 1;
            bindingInfo.bindings[i].descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            bindingInfo.bindings[i].pImmutableSamplers = nullptr;
            bindingInfo.bindings[i].stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;
        }

        bindingInfo.bindingCount = AttachmentCount;
        descriptorSetLayout      = renderer.vulkanState().descriptorPool.createLayout(bindingInfo);
    }

    virtual std::unique_ptr<DescriptorSetInstance> createDescriptorSet() const override {
        return std::make_unique<InputAttachmentInstance>(
            *vs, descriptorSetLayout, AttachmentCount, StartIndex);
    }

    virtual std::type_index creates() const override { return typeid(InputAttachmentInstance); }
};

} // namespace ds
} // namespace rc
} // namespace bl

#endif