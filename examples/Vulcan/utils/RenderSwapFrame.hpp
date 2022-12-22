#ifndef RENDERSWAPFRAME_HPP
#define RENDERSWAPFRAME_HPP

#include <glad/vulkan.h>
#include <stdexcept>

struct RenderSwapFrame {
    VkDevice deviceInitedWith;
    VkCommandBuffer commandBuffer;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;

    RenderSwapFrame() = default;
    RenderSwapFrame(VkDevice device, VkCommandPool commandPool);
    ~RenderSwapFrame();

    void initialize(VkDevice device, VkCommandPool commandPool);
    void cleanup(VkDevice device);
};

#endif
