#include <BLIB/Render/Descriptors/Builtin/Scene3DFactory.hpp>

#include <BLIB/Render/Descriptors/Builtin/Scene3DInstance.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace gfx
{
namespace ds
{
Scene3DFactory::~Scene3DFactory() {}

void Scene3DFactory::init(engine::Engine&, Renderer& renderer) {
    vulkanState = &renderer.vulkanState();

    vk::DescriptorPool::SetBindingInfo bindingInfo;
    bindingInfo.bindingCount = 1;

    bindingInfo.bindings[0].binding         = 0;
    bindingInfo.bindings[0].descriptorCount = 1;
    bindingInfo.bindings[0].descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    bindingInfo.bindings[0].stageFlags      = VK_SHADER_STAGE_VERTEX_BIT;

    descriptorSetLayout = vulkanState->descriptorPool.createLayout(bindingInfo);
}

std::unique_ptr<DescriptorSetInstance> Scene3DFactory::createDescriptorSet() const {
    return std::make_unique<Scene3DInstance>(*vulkanState, descriptorSetLayout);
}

} // namespace ds
} // namespace gfx
} // namespace bl
