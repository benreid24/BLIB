#include <BLIB/Render/Descriptors/Builtin/SlideshowFactory.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Render/Descriptors/Builtin/SlideshowInstance.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace ds
{
void SlideshowFactory::init(engine::Engine& e, Renderer& renderer) {
    engine = &e;

    vk::DescriptorPool::SetBindingInfo bindings;
    bindings.bindingCount = 4;

    for (std::uint32_t i = 0; i < bindings.bindingCount; ++i) {
        auto& binding              = bindings.bindings[i];
        binding.binding            = i;
        binding.descriptorCount    = 1;
        binding.descriptorType     = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        binding.stageFlags         = VK_SHADER_STAGE_VERTEX_BIT;
        binding.pImmutableSamplers = nullptr;
    }

    descriptorSetLayout = renderer.vulkanState().descriptorPool.createLayout(bindings);
}

std::unique_ptr<DescriptorSetInstance> SlideshowFactory::createDescriptorSet() const {
    return std::make_unique<SlideshowInstance>(*engine);
}

} // namespace ds
} // namespace rc
} // namespace bl
