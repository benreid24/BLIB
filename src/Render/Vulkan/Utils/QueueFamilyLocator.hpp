#ifndef BLIB_RENDER_VULKAN_UTILS_QUEUEFAMILYLOCATOR_HPP
#define BLIB_RENDER_VULKAN_UTILS_QUEUEFAMILYLOCATOR_HPP

#include <BLIB/Vulkan.hpp>
#include <cstdint>
#include <optional>

namespace bl
{
namespace rc
{
struct QueueFamilyLocator {
    std::optional<std::uint32_t> graphicsFamily;
    std::optional<std::uint32_t> presentFamily;

    QueueFamilyLocator() = default;
    QueueFamilyLocator(VkPhysicalDevice device, VkSurfaceKHR surface);

    void populate(VkPhysicalDevice device, VkSurfaceKHR surface);
    bool complete() const;
};

} // namespace rc
} // namespace bl

#endif
