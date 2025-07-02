#include <BLIB/Render/Descriptors/Builtin/InputAttachmentFactory.hpp>

#include <BLIB/Render/Descriptors/Builtin/InputAttachmentInstance.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace ds
{

void InputAttachmentFactory::init(engine::Engine&, Renderer& renderer) {
    vs = &renderer.vulkanState();

    vk::DescriptorPool::SetBindingInfo bindingInfo;

    bindingInfo.bindings[0].binding            = 0;
    bindingInfo.bindings[0].descriptorCount    = 1;
    bindingInfo.bindings[0].descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindingInfo.bindings[0].pImmutableSamplers = nullptr;
    bindingInfo.bindings[0].stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;
    bindingInfo.bindingCount                   = 1;
    descriptorSetLayout = renderer.vulkanState().descriptorPool.createLayout(bindingInfo);
}
std::unique_ptr<DescriptorSetInstance> InputAttachmentFactory::createDescriptorSet() const {
    return std::make_unique<InputAttachmentInstance>(*vs, descriptorSetLayout);
}

std::type_index InputAttachmentFactory::creates() const { return typeid(InputAttachmentInstance); }

} // namespace ds
} // namespace rc
} // namespace bl
