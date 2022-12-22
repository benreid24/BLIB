#include "QueueFamilyLocator.hpp"

QueueFamilyLocator::QueueFamilyLocator(VkPhysicalDevice device, VkSurfaceKHR surface) {
    populate(device, surface);
}

void QueueFamilyLocator::populate(VkPhysicalDevice device, VkSurfaceKHR surface) {
    std::uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    // find queues
    for (unsigned int i = 0; i < queueFamilies.size(); ++i) {
        // graphics queue
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) { graphicsFamily = i; }

        // presentation support
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        if (presentSupport) { presentFamily = i; }

        if (complete()) break;
    }
}

bool QueueFamilyLocator::complete() const {
    return graphicsFamily.has_value() && presentFamily.has_value();
}
