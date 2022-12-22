#ifndef RENDERERSTATE_HPP
#define RENDERERSTATE_HPP

#include "RenderSwapFrame.hpp"
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

    RenderSwapFrame* beginFrame(VkRenderPassBeginInfo& renderPassInfo);
    void completeFrame();

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
    std::array<RenderSwapFrame, MaxConcurrentFrames> renderFrames;

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

    void recreateSwapChain();

    void cleanupSwapchain();
    void cleanupDebugMessenger();
};

#endif
