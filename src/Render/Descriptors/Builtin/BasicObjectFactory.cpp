#include <BLIB/Render/Descriptors/Builtin/BasicObjectFactory.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Render/Descriptors/Builtin/BasicObjectInstance.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace render
{
namespace ds
{
void BasicObjectFactory::init(engine::Engine& e, Renderer& renderer) {
    engine = &e;

    // create descriptor layout
    vk::DescriptorPool::SetBindingInfo bindings;
    bindings.bindingCount = 2;

    bindings.bindings[0].binding         = 0;
    bindings.bindings[0].descriptorCount = 1;
    bindings.bindings[0].descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    bindings.bindings[0].stageFlags      = VK_SHADER_STAGE_VERTEX_BIT;

    bindings.bindings[1].binding         = 1;
    bindings.bindings[1].descriptorCount = 1;
    bindings.bindings[1].descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    bindings.bindings[1].stageFlags      = VK_SHADER_STAGE_FRAGMENT_BIT;

    descriptorSetLayout = renderer.vulkanState().descriptorPool.createLayout(bindings);
}

std::unique_ptr<DescriptorSetInstance> BasicObjectFactory::createDescriptorSet() const {
    return std::make_unique<BasicObjectInstance>(*engine, descriptorSetLayout);
}

} // namespace ds
} // namespace render
} // namespace bl
