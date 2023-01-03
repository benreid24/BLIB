#ifndef BLIB_RENDER_VULKAN_UTILS_SWAPCHAINSUPPORTDETAILS_HPP
#define BLIB_RENDER_VULKAN_UTILS_SWAPCHAINSUPPORTDETAILS_HPP

#include <BLIB/Logging.hpp>
#include <glad/vulkan.h>
#include <vector>

namespace bl
{
namespace render
{
struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;

    SwapChainSupportDetails() = default;
    SwapChainSupportDetails(VkPhysicalDevice device, VkSurfaceKHR surface);

    void populate(VkPhysicalDevice device, VkSurfaceKHR surface);
    const VkSurfaceFormatKHR& swapSurfaceFormat() const;
    VkPresentModeKHR presentationMode() const;
    VkExtent2D swapExtent(const sf::Vector2u& windowSize) const;
};

} // namespace render
} // namespace bl

#endif
