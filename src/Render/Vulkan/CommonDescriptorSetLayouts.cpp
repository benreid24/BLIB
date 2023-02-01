#include <BLIB/Render/Vulkan/CommonDescriptorSetLayouts.hpp>

#include <BLIB/Render/Overlays/Drawables/Image.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>

namespace bl
{
namespace render
{
void CommonDescriptorSetLayouts::init(VulkanState& vs) {
    // image overlay
    const auto layoutBindings = overlay::Image::DescriptorLayoutBindings();
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<std::uint32_t>(layoutBindings.size());
    layoutInfo.pBindings    = layoutBindings.data();
    if (vkCreateDescriptorSetLayout(vs.device, &layoutInfo, nullptr, &imageOverlay) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create overlay::Image descriptor set layout");
    }
}

void CommonDescriptorSetLayouts::cleanup(VulkanState& vs) {
    vkDestroyDescriptorSetLayout(vs.device, imageOverlay, nullptr);
}

} // namespace render
} // namespace bl
