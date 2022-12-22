#ifndef QUEUEFAMILYLOCATOR_HPP
#define QUEUEFAMILYLOCATOR_HPP

#include <cstdint>
#include <glad/vulkan.h>
#include <optional>
#include <vector>

struct QueueFamilyLocator {
    std::optional<std::uint32_t> graphicsFamily;
    std::optional<std::uint32_t> presentFamily;

    QueueFamilyLocator() = default;
    QueueFamilyLocator(VkPhysicalDevice device, VkSurfaceKHR surface);

    void populate(VkPhysicalDevice device, VkSurfaceKHR surface);
    bool complete() const;
};

#endif
