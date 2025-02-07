#include <BLIB/Render/Descriptors/Builtin/Scene3DFactory.hpp>

#include <BLIB/Render/Descriptors/Builtin/Scene3DInstance.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace ds
{
Scene3DFactory::~Scene3DFactory() {}

void Scene3DFactory::init(engine::Engine&, Renderer& renderer) {
    vulkanState = &renderer.vulkanState();

    vk::DescriptorPool::SetBindingInfo bindingInfo;
    bindingInfo.bindingCount = 4;

    bindingInfo.bindings[0].binding         = 0;
    bindingInfo.bindings[0].descriptorCount = 1;
    bindingInfo.bindings[0].descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    bindingInfo.bindings[0].stageFlags      = VK_SHADER_STAGE_VERTEX_BIT;

    bindingInfo.bindings[1].binding         = 1;
    bindingInfo.bindings[1].descriptorCount = 1;
    bindingInfo.bindings[1].descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    bindingInfo.bindings[1].stageFlags      = VK_SHADER_STAGE_FRAGMENT_BIT;

    bindingInfo.bindings[2].binding         = 2;
    bindingInfo.bindings[2].descriptorCount = 1;
    bindingInfo.bindings[2].descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    bindingInfo.bindings[2].stageFlags      = VK_SHADER_STAGE_FRAGMENT_BIT;

    bindingInfo.bindings[3].binding         = 3;
    bindingInfo.bindings[3].descriptorCount = 1;
    bindingInfo.bindings[3].descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    bindingInfo.bindings[3].stageFlags      = VK_SHADER_STAGE_FRAGMENT_BIT;

    descriptorSetLayout = vulkanState->descriptorPool.createLayout(bindingInfo);
}

std::unique_ptr<DescriptorSetInstance> Scene3DFactory::createDescriptorSet() const {
    return std::make_unique<Scene3DInstance>(*vulkanState, descriptorSetLayout);
}

std::type_index Scene3DFactory::creates() const { return typeid(Scene3DInstance); }

} // namespace ds
} // namespace rc
} // namespace bl
