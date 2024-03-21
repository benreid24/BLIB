#include <BLIB/Render/Descriptors/Builtin/Scene2DFactory.hpp>

#include <BLIB/Render/Descriptors/Builtin/Scene2DInstance.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace ds
{
Scene2DFactory::~Scene2DFactory() {}

void Scene2DFactory::init(engine::Engine&, Renderer& renderer) {
    vulkanState = &renderer.vulkanState();

    vk::DescriptorPool::SetBindingInfo bindingInfo;
    bindingInfo.bindingCount = 2;

    bindingInfo.bindings[0].binding         = 0;
    bindingInfo.bindings[0].descriptorCount = 1;
    bindingInfo.bindings[0].descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    bindingInfo.bindings[0].stageFlags      = VK_SHADER_STAGE_VERTEX_BIT;

    bindingInfo.bindings[1].binding         = 1;
    bindingInfo.bindings[1].descriptorCount = 1;
    bindingInfo.bindings[1].descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    bindingInfo.bindings[1].stageFlags      = VK_SHADER_STAGE_FRAGMENT_BIT;

    descriptorSetLayout = vulkanState->descriptorPool.createLayout(bindingInfo);
}

std::unique_ptr<DescriptorSetInstance> Scene2DFactory::createDescriptorSet() const {
    return std::make_unique<Scene2DInstance>(*vulkanState, descriptorSetLayout);
}

} // namespace ds
} // namespace rc
} // namespace bl
