#ifndef BLIB_RENDER_VULKAN_SWAPCHAIN_HPP
#define BLIB_RENDER_VULKAN_SWAPCHAIN_HPP

#include <BLIB/Render/Vulkan/Framebuffer.hpp>
#include <glad/vulkan.h>
#include <vector>
#include <BLIB/Render/Util/PerFrame.hpp>
#include <SFML/Window/WindowBase.hpp>

namespace bl
{
namespace render
{
struct VulkanState;

class Swapchain {
public:
    Swapchain(VulkanState& vulkanState, sf::WindowBase& window);

    void destroy();
    
    void create(VkSurfaceKHR surface, VkRenderPass renderPass);

    void invalidate();

    VkCommandBuffer beginFrame();

    void completeFrame();

private:
    VulkanState& vulkanState;
    sf::WindowBase& window;
    VkSurfaceKHR surface;
    VkRenderPass renderPass;

    VkSwapchainKHR swapchain;
    VkFormat imageFormat;
    VkExtent2D extent;
    std::vector<VkImage> images;
    std::vector<VkImageView> imageViews;
    std::vector<Framebuffer> framebuffers;
    PerFrame<VkFence> frameFences;
    PerFrame<VkSemaphore> imageAvailableSemaphores;
    PerFrame<VkSemaphore> frameFinishedSemaphores;
    std::uint32_t currentImageIndex;
    bool outOfDate;

    void cleanup();
    void recreate();
    void createSwapchain();
    void createImageViews();
    void createFramebuffers();
};

} // namespace render
} // namespace bl

#endif
