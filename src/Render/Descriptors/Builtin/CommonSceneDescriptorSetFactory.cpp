#include <BLIB/Render/Descriptors/Builtin/CommonSceneDescriptorSetFactory.hpp>

#include <BLIB/Render/Descriptors/Builtin/CommonSceneDescriptorSetInstance.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace render
{
namespace ds
{
CommonSceneDescriptorSetFactory::~CommonSceneDescriptorSetFactory() {}

void CommonSceneDescriptorSetFactory::init(engine::Engine&, Renderer& renderer) {
    vulkanState = &renderer.vulkanState();

    vk::DescriptorPool::SetBindingInfo bindingInfo;
    bindingInfo.bindingCount = 1;

    bindingInfo.bindings[0].binding         = 0;
    bindingInfo.bindings[0].descriptorCount = 1;
    bindingInfo.bindings[0].descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    bindingInfo.bindings[0].stageFlags      = VK_SHADER_STAGE_VERTEX_BIT;

    descriptorSetLayout = vulkanState->descriptorPool.createLayout(bindingInfo);
}

std::unique_ptr<DescriptorSetInstance> CommonSceneDescriptorSetFactory::createDescriptorSet()
    const {
    return std::make_unique<CommonSceneDescriptorSetInstance>(*vulkanState, descriptorSetLayout);
}

} // namespace ds
} // namespace render
} // namespace bl
