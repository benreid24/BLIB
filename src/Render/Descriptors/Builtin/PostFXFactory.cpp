#include <BLIB/Render/Descriptors/Builtin/PostFXFactory.hpp>

#include <BLIB/Render/Renderer.hpp>
#include <BLIB/Render/Scenes/PostFX.hpp>

namespace bl
{
namespace render
{
namespace ds
{

void PostFXFactory::init(engine::Engine&, Renderer& renderer) {
    vk::DescriptorPool::SetBindingInfo bindingInfo;
    bindingInfo.bindings[0]  = scene::PostFX::DescriptorLayoutBindings()[0];
    bindingInfo.bindingCount = 1;
    descriptorSetLayout      = renderer.vulkanState().descriptorPool.createLayout(bindingInfo);
}
std::unique_ptr<DescriptorSetInstance> PostFXFactory::createDescriptorSet() const {
    throw std::runtime_error("No instances should be created for PostFXFactory");
}

//
} // namespace ds
} // namespace render
} // namespace bl
