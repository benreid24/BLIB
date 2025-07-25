#include <BLIB/Render/Descriptors/Builtin/Scene3DFactory.hpp>

#include <BLIB/Render/Descriptors/Builtin/Scene3DInstance.hpp>
#include <BLIB/Render/Descriptors/SceneDescriptorSetInstance.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace ds
{
Scene3DFactory::~Scene3DFactory() {}

void Scene3DFactory::init(engine::Engine&, Renderer& r) {
    renderer    = &r;
    vulkanState = &r.vulkanState();

    vk::DescriptorPool::SetBindingInfo bindingInfo;
    bindingInfo.bindingCount = 5;

    // camera info
    bindingInfo.bindings[0] = SceneDescriptorSetInstance::getCameraBufferBindingInfo(
        0, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);

    // sunlight & spot lights & point lights & global lighting
    bindingInfo.bindings[1].descriptorCount = 1;
    bindingInfo.bindings[1].descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    bindingInfo.bindings[1].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

    // spot light shadow maps
    bindingInfo.bindings[2].descriptorCount = cfg::Limits::MaxSpotShadows;
    bindingInfo.bindings[2].descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindingInfo.bindings[2].stageFlags      = VK_SHADER_STAGE_FRAGMENT_BIT;

    // point light shadow maps
    bindingInfo.bindings[3].descriptorCount = cfg::Limits::MaxPointShadows;
    bindingInfo.bindings[3].descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindingInfo.bindings[3].stageFlags      = VK_SHADER_STAGE_FRAGMENT_BIT;

    // ssao buffer
    bindingInfo.bindings[4].descriptorCount = 1;
    bindingInfo.bindings[4].descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindingInfo.bindings[4].stageFlags      = VK_SHADER_STAGE_FRAGMENT_BIT;

    descriptorSetLayout = vulkanState->descriptorPool.createLayout(bindingInfo);
}

std::unique_ptr<DescriptorSetInstance> Scene3DFactory::createDescriptorSet() const {
    return std::make_unique<Scene3DInstance>(*renderer, descriptorSetLayout);
}

std::type_index Scene3DFactory::creates() const { return typeid(Scene3DInstance); }

} // namespace ds
} // namespace rc
} // namespace bl
