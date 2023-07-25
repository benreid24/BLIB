#include <BLIB/Render/Descriptors/Builtin/FadeEffectFactory.hpp>

#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace ds
{

void FadeEffectFactory::init(engine::Engine&, Renderer& renderer) {
    vk::DescriptorPool::SetBindingInfo bindingInfo;

    // sampled image (fade factor is float push constant)
    bindingInfo.bindings[0].binding            = 0;
    bindingInfo.bindings[0].descriptorCount    = 1;
    bindingInfo.bindings[0].descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindingInfo.bindings[0].pImmutableSamplers = nullptr;
    bindingInfo.bindings[0].stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;
    bindingInfo.bindingCount                   = 1;
    descriptorSetLayout = renderer.vulkanState().descriptorPool.createLayout(bindingInfo);
}
std::unique_ptr<DescriptorSetInstance> FadeEffectFactory::createDescriptorSet() const {
    throw std::runtime_error("No instances should be created for FadeEffectFactory");
}

} // namespace ds
} // namespace rc
} // namespace bl
