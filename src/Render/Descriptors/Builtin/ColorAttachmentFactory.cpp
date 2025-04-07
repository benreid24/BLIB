#include <BLIB/Render/Descriptors/Builtin/ColorAttachmentFactory.hpp>

#include <BLIB/Render/Descriptors/Builtin/ColorAttachmentInstance.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace ds
{

void ColorAttachmentFactory::init(engine::Engine&, Renderer& renderer) {
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
std::unique_ptr<DescriptorSetInstance> ColorAttachmentFactory::createDescriptorSet() const {
    return std::make_unique<ColorAttachmentInstance>(*vs, descriptorSetLayout);
}

std::type_index ColorAttachmentFactory::creates() const { return typeid(ColorAttachmentInstance); }

} // namespace ds
} // namespace rc
} // namespace bl
