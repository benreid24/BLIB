#include <BLIB/Render/Descriptors/Builtin/Scene2DFactory.hpp>

#include <BLIB/Render/Descriptors/Builtin/Scene2DInstance.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace dsi
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

    descriptorSetLayout = renderer.getDescriptorPool().createLayout(bindingInfo);
}

std::unique_ptr<ds::DescriptorSetInstance> Scene2DFactory::createDescriptorSet() const {
    return std::make_unique<Scene2DInstance>(*vulkanState, descriptorSetLayout);
}

std::type_index Scene2DFactory::creates() const { return typeid(Scene2DInstance); }

} // namespace dsi
} // namespace rc
} // namespace bl
