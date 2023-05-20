#include <BLIB/Render/Descriptors/Builtin/Scene2DLitFactory.hpp>

#include <BLIB/Render/Descriptors/Builtin/Scene2DLitInstance.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace render
{
namespace ds
{
Scene2DLitFactory::~Scene2DLitFactory() {}

void Scene2DLitFactory::init(engine::Engine&, Renderer& renderer) {
    vulkanState = &renderer.vulkanState();

    vk::DescriptorPool::SetBindingInfo bindingInfo;
    bindingInfo.bindingCount = 1;

    bindingInfo.bindings[0].binding         = 0;
    bindingInfo.bindings[0].descriptorCount = 1;
    bindingInfo.bindings[0].descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    bindingInfo.bindings[0].stageFlags      = VK_SHADER_STAGE_VERTEX_BIT;

    descriptorSetLayout = vulkanState->descriptorPool.createLayout(bindingInfo);
}

std::unique_ptr<DescriptorSetInstance> Scene2DLitFactory::createDescriptorSet() const {
    return std::make_unique<Scene2DLitInstance>(*vulkanState, descriptorSetLayout);
}

} // namespace ds
} // namespace render
} // namespace bl
