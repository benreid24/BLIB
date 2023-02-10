#include <BLIB/Render/Vulkan/CommonDescriptorSetLayouts.hpp>

#include <BLIB/Render/Overlays/Drawables/Image.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>

namespace bl
{
namespace render
{
void CommonDescriptorSetLayouts::init(VulkanState& vs) {
    // image overlay
    static const auto imageOverlayBindings = overlay::Image::DescriptorLayoutBindings();
    imageOverlay.createInfo.sType          = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    imageOverlay.createInfo.bindingCount = static_cast<std::uint32_t>(imageOverlayBindings.size());
    imageOverlay.createInfo.pBindings    = imageOverlayBindings.data();
    if (vkCreateDescriptorSetLayout(
            vs.device, &imageOverlay.createInfo, nullptr, &imageOverlay.layout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create overlay::Image descriptor set layout");
    }
}

void CommonDescriptorSetLayouts::cleanup(VulkanState& vs) {
    vkDestroyDescriptorSetLayout(vs.device, imageOverlay.layout, nullptr);
}

CommonDescriptorSetLayouts::Layout::Layout()
: createInfo{}
, layout(nullptr) {}

} // namespace render
} // namespace bl
