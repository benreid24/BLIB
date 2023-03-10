#ifndef BLIB_RENDER_VULKAN_VULKANSTATE_HPP
#define BLIB_RENDER_VULKAN_VULKANSTATE_HPP

#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Transfers/TransferEngine.hpp>
#include <BLIB/Render/Util/PerFrame.hpp>
#include <BLIB/Render/Vulkan/CommonDescriptorSetLayouts.hpp>
#include <BLIB/Render/Vulkan/CommonSamplers.hpp>
#include <BLIB/Render/Vulkan/DescriptorPool.hpp>
#include <BLIB/Render/Vulkan/Framebuffer.hpp>
#include <BLIB/Render/Vulkan/Swapchain.hpp>
#include <SFML/Window.hpp>
#include <array>
#include <cstdint>
#include <glad/vulkan.h>
#include <initializer_list>
#include <stdexcept>
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
     * @brief Destroy the Vulkan State object
     *
     */
    ~VulkanState();

    /**
     * @brief Marks the swap chain as invalid. Called when the window is resized or recreated
     *
     */
    void invalidateSwapChain();

    /**
     * @brief Begins a render pass on the current swap chain image. Returns the command buffer to
     *        use for rendering
     *
     * @param renderFrame A reference to a pointer to populate with the active chain image
     * @param commandBuffer A command buffer reference to populate with the primary CB to use
     */
    void beginFrame(ColorAttachmentSet*& renderFrame, VkCommandBuffer& commandBuffer);

    /**
     * @brief Finalizes the render pass and command buffer for the current frame and submits it.
     *        Also triggers the swap chain present operation
     *
     */
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
     * @brief Helper method to create a command pool for graphics queue commands
     *
     * @param flags The flags to create the pool with
     * @return VkCommandPool The new command pool
     */
    VkCommandPool createCommandPool(VkCommandPoolCreateFlags flags);

    /**
     * @brief Helper function to create a command buffer optimized for one-off single commands such
     *        as transfers
     *
     * @param commandPool Optional command pool to use to allocate the command buffer
     * @return VkCommandBuffer A new command buffer for one-off commands
     */
    VkCommandBuffer beginSingleTimeCommands(VkCommandPool commandPool = nullptr);

    /**
     * @brief Finalizes the given command buffer and frees it
     *
     * @param commandBuffer The command buffer to submit and free
     * @param commandPool The command pool used to allocate the buffer
     */
    void endSingleTimeCommands(VkCommandBuffer commandBuffer, VkCommandPool commandPool = nullptr);

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
     * @brief Helper function to find the best-suited image format given the requested format and
     *        current hardware support
     *
     * @param candidates Acceptable image formats ordered by preference
     * @param tiling The tiling mode of the image
     * @param features Required features of the format
     * @return The best format available
     */
    VkFormat findSupportedFormat(const std::initializer_list<VkFormat>& candidates,
                                 VkImageTiling tiling, VkFormatFeatureFlags features);

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
     * @param aspectFlags Flags to use when creating the view
     * @return VkImageView The created image view
     */
    VkImageView createImageView(VkImage image, VkFormat format,
                                VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT);

    /**
     * @brief Creates a shader module from the given shader path
     *
     * @param path The resource path of the shader to load
     * @return VkShaderModule The created shader module
     */
    VkShaderModule createShaderModule(const std::string& path);

    /**
     * @brief Returns the current frame index. Used by PerFrame to return correct data
     *
     * @return constexpr std::uint32_t The current index of the frame being worked on
     */
    constexpr std::uint32_t currentFrameIndex() const;

    VkInstance instance;
#ifdef BLIB_DEBUG
    VkDebugUtilsMessengerEXT debugMessenger;
#endif
    VkSurfaceKHR surface;
    VkPhysicalDevice physicalDevice;
    VkPhysicalDeviceProperties physicalDeviceProperties;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkCommandPool sharedCommandPool;
    Swapchain swapchain;
    tfr::TransferEngine transferEngine;
    DescriptorPool descriptorPool;
    CommonDescriptorSetLayouts descriptorSetLayouts;
    CommonSamplers samplerCache;

private:
    sf::WindowBase& window;
    std::uint32_t currentFrame;

    VulkanState(sf::WindowBase& window);
    void init();
    void cleanup();

    void createInstance();
    void setupDebugMessenger();
    void createSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createSharedCommandPool();

    void cleanupDebugMessenger();

    friend class Renderer;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr std::uint32_t VulkanState::currentFrameIndex() const { return currentFrame; }

template<typename T>
PerFrame<T>::PerFrame()
: vs(nullptr) {}

template<typename T>
template<typename U>
void PerFrame<T>::init(VulkanState& v, const U& visitor) {
    vs = &v;
    for (T& o : data) { visitor(o); }
}

template<typename T>
template<typename U>
void PerFrame<T>::cleanup(const U& visitor) {
    for (T& o : data) { visitor(o); }
}

template<typename T>
constexpr T& PerFrame<T>::current() {
#ifdef BLIB_DEBUG
    if (!vs) { throw std::runtime_error("PerFrame has not been inited with VulkanInstance"); }
#endif
    return data[vs->currentFrameIndex()];
}

template<typename T>
constexpr const T& PerFrame<T>::current() const {
#ifdef BLIB_DEBUG
    if (!vs) { throw std::runtime_error("PerFrame has not been inited with VulkanInstance"); }
#endif
    return data[vs->currentFrameIndex()];
}

template<typename T>
constexpr T& PerFrame<T>::getRaw(unsigned int i) {
    return data[i];
}

template<typename T>
constexpr const T& PerFrame<T>::getRaw(unsigned int i) const {
    return data[i];
}

template<typename T>
constexpr bool PerFrame<T>::valid() const {
    return vs != nullptr;
}

template<typename T>
constexpr std::size_t PerFrame<T>::size() const {
    return data.size();
}

template<typename T>
constexpr T* PerFrame<T>::rawData() {
    return data.data();
}

template<typename T>
constexpr const T* PerFrame<T>::rawData() const {
    return data.data();
}

} // namespace render
} // namespace bl

#endif
