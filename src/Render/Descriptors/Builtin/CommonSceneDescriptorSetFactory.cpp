#include <BLIB/Render/Descriptors/Builtin/CommonSceneDescriptorSetFactory.hpp>

#include <BLIB/Render/Descriptors/Builtin/CommonSceneDescriptorSetInstance.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace render
{
namespace ds
{
CommonSceneDescriptorSetFactory::~CommonSceneDescriptorSetFactory() {
    if (vulkanState) {
        vkDestroyDescriptorSetLayout(vulkanState->device, descriptorSetLayout, nullptr);
    }
}

void CommonSceneDescriptorSetFactory::init(engine::Engine&, Renderer& renderer) {
    vulkanState = &renderer.vulkanState();

    VkDescriptorSetLayoutBinding cameraBinding{};
    cameraBinding.binding         = 0;
    cameraBinding.descriptorCount = 1;
    cameraBinding.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    cameraBinding.stageFlags      = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutBinding setBindings[] = {cameraBinding};
    VkDescriptorSetLayoutCreateInfo descriptorCreateInfo{};
    descriptorCreateInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorCreateInfo.bindingCount = std::size(setBindings);
    descriptorCreateInfo.pBindings    = setBindings;
    if (VK_SUCCESS !=
        vkCreateDescriptorSetLayout(
            vulkanState->device, &descriptorCreateInfo, nullptr, &descriptorSetLayout)) {
        throw std::runtime_error("Failed to create descriptor set layout");
    }
}

std::unique_ptr<DescriptorSetInstance> CommonSceneDescriptorSetFactory::createDescriptorSet()
    const {
    return std::make_unique<CommonSceneDescriptorSetInstance>(*vulkanState, descriptorSetLayout);
}

} // namespace ds
} // namespace render
} // namespace bl
