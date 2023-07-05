#ifndef BLIB_RENDER_VULKAN_UTILS_SWAPCHAINSUPPORTDETAILS_HPP
#define BLIB_RENDER_VULKAN_UTILS_SWAPCHAINSUPPORTDETAILS_HPP

#include <BLIB/Logging.hpp>
#include <volk.h>
#include <vector>

namespace bl
{
namespace gfx
{
struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;

    SwapChainSupportDetails() = default;
    SwapChainSupportDetails(VkPhysicalDevice device, VkSurfaceKHR surface);

    void populate(VkPhysicalDevice device, VkSurfaceKHR surface);
    const VkSurfaceFormatKHR& swapSurfaceFormat() const;
    VkPresentModeKHR presentationMode(bool disableVsync) const;
    VkExtent2D swapExtent(const sf::Vector2u& windowSize) const;
};

} // namespace gfx
} // namespace bl

#endif
