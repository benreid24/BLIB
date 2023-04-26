#include <BLIB/Render/Descriptors/Builtin/DefaultObjectDescriptorSetFactory.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Render/Descriptors/Builtin/DefaultObjectDescriptorSetInstance.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace render
{
namespace ds
{
DefaultObjectDescriptorSetFactory::~DefaultObjectDescriptorSetFactory() {
    if (engine) {
        vkDestroyDescriptorSetLayout(
            engine->renderer().vulkanState().device, descriptorSetLayout, nullptr);
    }
}

void DefaultObjectDescriptorSetFactory::init(engine::Engine& e, Renderer& renderer) {
    engine = &e;

    // create descriptor layout
    VkDescriptorSetLayoutBinding transformBinding{};
    transformBinding.binding         = 0;
    transformBinding.descriptorCount = 1;
    transformBinding.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    transformBinding.stageFlags      = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutBinding textureBinding{};
    textureBinding.binding         = 1;
    textureBinding.descriptorCount = 1;
    textureBinding.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    textureBinding.stageFlags      = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutBinding setBindings[] = {transformBinding, textureBinding};
    VkDescriptorSetLayoutCreateInfo descriptorCreateInfo{};
    descriptorCreateInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorCreateInfo.bindingCount = std::size(setBindings);
    descriptorCreateInfo.pBindings    = setBindings;
    if (VK_SUCCESS !=
        vkCreateDescriptorSetLayout(
            renderer.vulkanState().device, &descriptorCreateInfo, nullptr, &descriptorSetLayout)) {
        throw std::runtime_error("Failed to create descriptor set layout");
    }
}

std::unique_ptr<DescriptorSetInstance> DefaultObjectDescriptorSetFactory::createDescriptorSet()
    const {
    return std::make_unique<DefaultObjectDescriptorSetInstance>(*engine, descriptorSetLayout);
}

} // namespace ds
} // namespace render
} // namespace bl
