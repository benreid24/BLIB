#ifndef RENDERERSTATE_HPP
#define RENDERERSTATE_HPP

#include "RenderSwapFrame.hpp"
#include "TransformUniform.hpp"
#include <SFML/Window.hpp>
#include <array>
#include <cstdint>
#include <glad/vulkan.h>
#include <vector>

struct RendererState {
    static constexpr unsigned int MaxConcurrentFrames = 2;

    RendererState(sf::WindowBase& window);
    ~RendererState();

    void finalizeInitialization(VkRenderPass renderPass);

    void invalidateSwapChain();

    void updateUniforms(const TransformUniform& tform);
    RenderSwapFrame* beginFrame(VkRenderPassBeginInfo& renderPassInfo, VkDescriptorSet& descriptorSet);
    void completeFrame();

    std::uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                      VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    VkInstance instance;
#ifdef BLIB_DEBUG
    VkDebugUtilsMessengerEXT debugMessenger;
#endif
    VkSurfaceKHR surface;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;
    VkCommandPool commandPool;
    VkCommandBuffer transferCommandBuffer;
    std::array<RenderSwapFrame, MaxConcurrentFrames> renderFrames;

    VkDescriptorSetLayout descriptorSetLayout;
    std::array<VkBuffer, MaxConcurrentFrames> uniformBuffers;
    std::array<VkDeviceMemory, MaxConcurrentFrames> uniformBuffersMemory;
    std::array<void*, MaxConcurrentFrames> uniformBuffersMapped;
    VkDescriptorPool descriptorPool;
    std::array<VkDescriptorSet, MaxConcurrentFrames> descriptorSets;

private:
    sf::WindowBase& window;
    VkRenderPass renderPass;
    std::uint32_t currentFrame;
    std::uint32_t currentRenderImageIndex;
    bool swapChainOutOfDate;

    void createInstance();
    void setupDebugMessenger();
    void createSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createSwapChain();
    void createImageViews();
    void createFramebuffers();
    void createCommandPool();
    void createRenderFrames();
    void createDescriptorSetLayout();
    void createUniformBuffers();
    void createDescriptorPool();
    void createDescriptorSets();

    void recreateSwapChain();

    void cleanupSwapchain();
    void cleanupDebugMessenger();
};

#endif
