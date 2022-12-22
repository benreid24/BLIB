#ifndef SWAPCHAINSUPPORTDETAILS_HPP
#define SWAPCHAINSUPPORTDETAILS_HPP

#include <BLIB/Logging.hpp>
#include <glad/vulkan.h>
#include <vector>

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

#endif
