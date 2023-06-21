#include <BLIB/Render/Descriptors/Builtin/Object3DFactory.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Render/Descriptors/Builtin/Object3DInstance.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace gfx
{
namespace ds
{
void Object3DFactory::init(engine::Engine& e, Renderer& renderer) {
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

std::unique_ptr<DescriptorSetInstance> Object3DFactory::createDescriptorSet() const {
    return std::make_unique<Object3DInstance>(*engine, descriptorSetLayout);
}

} // namespace ds
} // namespace gfx
} // namespace bl
