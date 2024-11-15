#include <Render/Vulkan/Utils/SwapChainSupportDetails.hpp>

#include <BLIB/Render/Vulkan/VkCheck.hpp>

namespace bl
{
namespace rc
{
SwapChainSupportDetails::SwapChainSupportDetails(VkPhysicalDevice device, VkSurfaceKHR surface) {
    populate(device, surface);
}

void SwapChainSupportDetails::populate(VkPhysicalDevice device, VkSurfaceKHR surface) {
    vkCheck(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &capabilities));

    std::uint32_t formatCount = 0;
    vkCheck(vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr));
    if (formatCount != 0) {
        formats.resize(formatCount);
        vkCheck(
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, formats.data()));
    }

    std::uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
    if (presentModeCount != 0) {
        presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            device, surface, &presentModeCount, presentModes.data());
    }
}

const VkSurfaceFormatKHR& SwapChainSupportDetails::swapSurfaceFormat() const {
    for (const VkSurfaceFormatKHR& format : formats) {
        if (format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR &&
            format.format == VK_FORMAT_B8G8R8A8_UNORM) {
            return format;
        }
    }
    BL_LOG_DEBUG << "Did not find VK_FORMAT_B8G8R8A8_UNORM, using " << formats.front().format;
    return formats.front();
}

VkPresentModeKHR SwapChainSupportDetails::presentationMode(bool noVsync) const {
    const VkPresentModeKHR requestedMode =
        noVsync ? VK_PRESENT_MODE_IMMEDIATE_KHR : VK_PRESENT_MODE_MAILBOX_KHR;
    for (const VkPresentModeKHR mode : presentModes) {
        if (mode == requestedMode) { return mode; }
    }
    BL_LOG_DEBUG << "VK_PRESENT_MODE_MAILBOX_KHR not supported, falling back to "
                    "VK_PRESENT_MODE_FIFO_KHR";
    return VK_PRESENT_MODE_FIFO_KHR; // always available
}

VkExtent2D SwapChainSupportDetails::swapExtent(const sf::Vector2u& windowSize) const {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }
    else {
        VkExtent2D actualExtent = {windowSize.x, windowSize.y};

        actualExtent.width  = std::clamp(actualExtent.width,
                                        capabilities.minImageExtent.width,
                                        capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height,
                                         capabilities.minImageExtent.height,
                                         capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

} // namespace rc
} // namespace bl
