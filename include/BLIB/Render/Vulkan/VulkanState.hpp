#ifndef BLIB_RENDER_VULKAN_VULKANSTATE_HPP
#define BLIB_RENDER_VULKAN_VULKANSTATE_HPP

#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Vulkan/SwapFrame.hpp>
#include <SFML/Window.hpp>
#include <array>
#include <cstdint>
#include <glad/vulkan.h>
#include <vector>

namespace bl
{
namespace render
{
/**
 * @brief Primary owner and manager of the Vulkan renderer's state
 *
 * @ingroup Renderer
 */
struct VulkanState {
    /**
     * @brief Create and initialize the Vulkan renderer
     *
     * @param window The window to attach to
     */
    VulkanState(sf::WindowBase& window);

    /**
     * @brief Destroy the Vulkan State object
     *
     */
    ~VulkanState();

    // TODO - should the final render pass come down here?
    void finalizeInitialization(VkRenderPass renderPass);

    /**
     * @brief Marks the swap chain as invalid. Called when the window is resized or recreated
     *
     */
    void invalidateSwapChain();

    // TODO - still need these?
    SwapFrame* beginFrame(VkRenderPassBeginInfo& renderPassInfo);
    void completeFrame();

    /**
     * @brief Helper function to find the optimal available memory type for the given constraints
     *
     * @param typeFilter Requested memory type
     * @param properties Requested properties of the given memory
     * @return std::uint32_t Closest match to the requested memory type and features
     */
    std::uint32_t findMemoryType(std::uint32_t typeFilter, VkMemoryPropertyFlags properties);

    /**
     * @brief Helper function to create a command buffer optimized for one-off single commands such
     *        as transfers
     *
     * @return VkCommandBuffer A new command buffer for one-off commands
     */
    VkCommandBuffer beginSingleTimeCommands();

    /**
     * @brief Finalizes the given command buffer and frees it
     *
     * @param commandBuffer The command buffer to submit and free
     */
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);

    /**
     * @brief Helper function to create a VkBuffer with some parameters
     *
     * @param size The size of the buffer to create
     * @param usage How the buffer will be used
     * @param properties Requested properties of the buffer memory
     * @param buffer The buffer to populate
     * @param bufferMemory The buffer memory to populate
     */
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                      VkBuffer& buffer, VkDeviceMemory& bufferMemory);

    /**
     * @brief Helper function to copy one buffer to another. The copy operation must be valid
     *
     * @param srcBuffer The buffer to copy from
     * @param dstBuffer The buffer to copy to
     * @param size The size of the data to copy
     */
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    /**
     * @brief Helper function to create an image
     *
     * @param width The width of the image in pixels
     * @param height The height of the image in pixels
     * @param format The format of the pixel data
     * @param tiling The tiling setting for the created image
     * @param usage How the image will be used
     * @param properties Requested memory properties for where the image is stored
     * @param image The image to be populated
     * @param imageMemory The device memory to be populated
     */
    void createImage(std::uint32_t width, std::uint32_t height, VkFormat format,
                     VkImageTiling tiling, VkImageUsageFlags usage,
                     VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

    /**
     * @brief Converts an image from one layout to another
     *
     * @param image The image to convert
     * @param format The format of the image to convert
     * @param oldLayout The current layout to convert from
     * @param newLayout The layout to convert to
     */
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout,
                               VkImageLayout newLayout);

    /**
     * @brief Helper function to copy a raw buffer's contents into an image
     *
     * @param buffer The buffer to copy into the image
     * @param image The image to copy into
     * @param width The width of the image
     * @param height The height of the image
     */
    void copyBufferToImage(VkBuffer buffer, VkImage image, std::uint32_t width,
                           std::uint32_t height);

    /**
     * @brief Helper function to create an image view
     *
     * @param image The image to create the view on
     * @param format The format of the image
     * @return VkImageView The created image view
     */
    VkImageView createImageView(VkImage image, VkFormat format);

    /**
     * @brief Creates a shader module from the given shader path
     * 
     * @param path The resource path of the shader to load
     * @return VkShaderModule The created shader module
     */
    VkShaderModule createShaderModule(const std::string& path);

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
    std::array<SwapFrame, Config::MaxConcurrentFrames> renderFrames;

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

} // namespace render
} // namespace bl

#endif
