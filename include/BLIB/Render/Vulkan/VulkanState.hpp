#ifndef BLIB_RENDER_VULKAN_VULKANSTATE_HPP
#define BLIB_RENDER_VULKAN_VULKANSTATE_HPP

#include <BLIB/Engine/Window.hpp>
#include <BLIB/Render/Config/Limits.hpp>
#include <BLIB/Render/Resources/ShaderModuleCache.hpp>
#include <BLIB/Render/Transfers/TransferEngine.hpp>
#include <BLIB/Render/Vulkan/CleanupManager.hpp>
#include <BLIB/Render/Vulkan/CommonSamplers.hpp>
#include <BLIB/Render/Vulkan/DescriptorPool.hpp>
#include <BLIB/Render/Vulkan/Framebuffer.hpp>
#include <BLIB/Render/Vulkan/PerFrame.hpp>
#include <BLIB/Render/Vulkan/PerFrameVector.hpp>
#include <BLIB/Render/Vulkan/SharedCommandPool.hpp>
#include <BLIB/Render/Vulkan/Swapchain.hpp>
#include <BLIB/Render/Vulkan/VkCheck.hpp>
#include <BLIB/Vulkan.hpp>
#include <array>
#include <cstdint>
#include <initializer_list>
#include <mutex>
#include <stdexcept>
#include <utility>
#include <vector>

namespace bl
{
namespace rc
{
class Renderer;

/// Collection of interfaces and utilities directly related to Vulkan
namespace vk
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
    void beginFrame(StandardAttachmentSet*& renderFrame, VkCommandBuffer& commandBuffer);

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
     * @brief Performs a synchronized submission of the command buffer to the graphics queue
     *
     * @param submitInfo The command buffer(s) to submit
     * @param fence Optional fence to pass to the submit command
     * @return The result code from the submit command
     */
    VkResult submitCommandBuffer(const VkSubmitInfo& submitInfo, VkFence fence = VK_NULL_HANDLE);

    /**
     * @brief Helper function to create a VkBuffer with some parameters
     *
     * @param size The size of the buffer to create
     * @param usage How the buffer will be used
     * @param allocFlags Extra flags to pass into VMA when the allocation is created
     * @param properties Requested properties of the buffer memory
     * @param buffer The buffer to populate
     * @param vmaAlloc VMA allocation to be populated
     * @param vmaAllocInfo VMA allocation info to be populated
     */
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                      VmaAllocationCreateFlags allocFlags, VkMemoryPropertyFlags properties,
                      VkBuffer* buffer, VmaAllocation* vmaAlloc, VmaAllocationInfo* vmaAllocInfo);

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
     * @brief Finds and returns the best available depth format
     */
    VkFormat findDepthFormat();

    /**
     * @brief Finds and returns the best available depth format for shadow maps
     */
    VkFormat findShadowMapFormat();

    /**
     * @brief Helper method to guess the aspect of an image based on its format and usage
     *
     * @param format The image format
     * @param usage The image usage
     * @return The probable image aspect
     */
    static VkImageAspectFlags guessImageAspect(VkFormat format, VkImageUsageFlags usage);

    /**
     * @brief Helper function to create an image
     *
     * @param width The width of the image in pixels
     * @param height The height of the image in pixels
     * @param layerCount The number of image layers to create
     * @param format The format of the pixel data
     * @param tiling The tiling setting for the created image
     * @param usage How the image will be used
     * @param imageFlags Creation flags to create the image with
     * @param properties Requested memory properties for where the image is stored
     * @param image The image to be populated
     * @param vmaAlloc VMA allocation to be populated
     * @param vmaAllocInfo VMA allocation info to be populated
     * @param flags Optional flags to pass to VMA for memory alloc
     */
    void createImage(std::uint32_t width, std::uint32_t height, std::uint32_t layerCount,
                     VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
                     VkImageCreateFlags imageFlags, VkMemoryPropertyFlags properties,
                     VkImage* image, VmaAllocation* vmaAlloc, VmaAllocationInfo* vmaAllocInfo,
                     VmaAllocationCreateFlags flags = 0);

    /**
     * @brief Converts an image from one layout to another
     *
     * @param image The image to convert
     * @param oldLayout The current layout to convert from
     * @param newLayout The layout to convert to
     * @param layerCount The number of layers to transition
     * @param aspect The image aspect
     */
    void transitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout,
                               std::uint32_t layerCount  = 1,
                               VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT);

    /**
     * @brief Converts an image from one layout to another and uses an existing command buffer
     *
     * @param commandBuffer The command buffer to record the pipeline barrier into
     * @param image The image to convert
     * @param oldLayout The current layout to convert from
     * @param newLayout The layout to convert to
     * @param layerCount The number of layers to transition
     * @param aspect The image aspect
     */
    void transitionImageLayout(VkCommandBuffer commandbuffer, VkImage image,
                               VkImageLayout oldLayout, VkImageLayout newLayout,
                               std::uint32_t layerCount  = 1,
                               VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT);

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
     * @param layerCount The number of layers in the image
     * @param viewType The type of image view to create
     * @return VkImageView The created image view
     */
    VkImageView createImageView(VkImage image, VkFormat format,
                                VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT,
                                std::uint32_t layerCount       = 1,
                                VkImageViewType viewType       = VK_IMAGE_VIEW_TYPE_2D);

    /**
     * @brief Returns the current frame index. Used by PerFrame to return correct data
     *
     * @return constexpr std::uint32_t The current index of the frame being worked on
     */
    constexpr std::uint32_t currentFrameIndex() const;

    /**
     * @brief Computes the aligned size of data given its actual size and alignment requirement
     *
     * @param dataSize The size of the data to align
     * @param alignment The alignment the data needs
     * @return The aligned size of the data
     */
    static VkDeviceSize computeAlignedSize(VkDeviceSize dataSize, VkDeviceSize alignment);

    /**
     * @brief Retrieves the format properties of the physical device for the given format
     *
     * @param format The format to get features for
     * @return The supported format features for the current physical device
     */
    VkFormatProperties getFormatProperties(VkFormat format);

    /**
     * @brief Returns whether the given extension is available on the selected device
     *
     * @param extName The extension to check for
     * @return True if the extension is available, false otherwise
     */
    bool extensionIsAvailable(const char* extName) const;

    /**
     * @brief Returns the physical device properties of the current device
     */
    static const VkPhysicalDeviceProperties& getPhysicalDeviceProperties();

    engine::EngineWindow& window;
    VkInstance instance;
#ifdef BLIB_DEBUG
    VkDebugUtilsMessengerEXT debugMessenger;
#endif
    VkSurfaceKHR surface;
    VkPhysicalDevice physicalDevice;
    VkPhysicalDeviceProperties physicalDeviceProperties;
    VkDevice device;
    VmaAllocator vmaAllocator;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    SharedCommandPool sharedCommandPool;
    Swapchain swapchain;
    res::ShaderModuleCache shaderCache;
    tfr::TransferEngine transferEngine;
    DescriptorPool descriptorPool;
    CommonSamplers samplerCache;
    CleanupManager cleanupManager;

    std::mutex cbSubmitMutex;
    std::mutex bufferAllocMutex;
    std::mutex imageAllocMutex;

private:
    std::uint32_t currentFrame;
    std::vector<VkExtensionProperties> availableExtensions;

    VulkanState(engine::EngineWindow& window);
    void init();
    void cleanup();

    void createInstance();
    void setupDebugMessenger();
    void createSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createVmaAllocator();

    void cleanupDebugMessenger();

    friend class bl::rc::Renderer;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr std::uint32_t VulkanState::currentFrameIndex() const { return currentFrame; }

template<typename T>
PerFrame<T>::PerFrame()
: vs(nullptr) {}

template<typename T>
template<typename... TArgs>
PerFrame<T>::PerFrame(TArgs&&... args)
: vs(nullptr)
, data{T(std::forward<TArgs>(args)...), T(std::forward<TArgs>(args)...)} {}

template<typename T>
void PerFrame<T>::emptyInit(VulkanState& v) {
    vs = &v;
}

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
template<typename U>
void PerFrame<T>::visit(const U& visitor) {
    for (T& o : data) { visitor(o); }
}

template<typename T>
template<typename U>
U& PerFrame<T>::getOther(const T& member, PerFrame<U>& other) const {
    return other.getRaw(getIndex(member));
}

template<typename T>
T& PerFrame<T>::current() {
#ifdef BLIB_DEBUG
    if (!vs) { throw std::runtime_error("PerFrame has not been inited with VulkanInstance"); }
#endif
    return data[vs->currentFrameIndex()];
}

template<typename T>
const T& PerFrame<T>::current() const {
#ifdef BLIB_DEBUG
    if (!vs) { throw std::runtime_error("PerFrame has not been inited with VulkanInstance"); }
#endif
    return data[vs->currentFrameIndex()];
}

template<typename T>
T& PerFrame<T>::next() {
#ifdef BLIB_DEBUG
    if (!vs) { throw std::runtime_error("PerFrame has not been inited with VulkanInstance"); }
#endif
    return data[vs->currentFrameIndex() < data.size() - 1 ? vs->currentFrameIndex() + 1 : 0];
}

template<typename T>
const T& PerFrame<T>::next() const {
#ifdef BLIB_DEBUG
    if (!vs) { throw std::runtime_error("PerFrame has not been inited with VulkanInstance"); }
#endif
    return data[vs->currentFrameIndex() < data.size() - 1 ? vs->currentFrameIndex() + 1 : 0];
}

template<typename T>
std::uint32_t PerFrame<T>::getCurrentIndex() const {
    return vs->currentFrameIndex();
}

template<typename T>
std::uint32_t PerFrame<T>::getIndex(const T& member) const {
    return &member - data.data();
}

template<typename T>
T& PerFrame<T>::getRaw(unsigned int i) {
    return data[i];
}

template<typename T>
const T& PerFrame<T>::getRaw(unsigned int i) const {
    return data[i];
}

template<typename T>
bool PerFrame<T>::valid() const {
    return vs != nullptr;
}

template<typename T>
std::size_t PerFrame<T>::size() const {
    return data.size();
}

template<typename T>
T* PerFrame<T>::rawData() {
    return data.data();
}

template<typename T>
const T* PerFrame<T>::rawData() const {
    return data.data();
}

template<typename T>
PerFrameVector<T>::PerFrameVector()
: vs(nullptr) {}

template<typename T>
void PerFrameVector<T>::emptyInit(VulkanState& vulkanState, std::uint32_t capacity) {
    vs  = &vulkanState;
    cap = capacity;
    items.resize(capacity * cfg::Limits::MaxConcurrentFrames);
}

template<typename T>
template<typename U>
void PerFrameVector<T>::init(VulkanState& vulkanState, std::uint32_t capacity, const U& visitor) {
    emptyInit(vulkanState, capacity);
    cleanup(visitor);
}

template<typename T>
template<typename U>
void PerFrameVector<T>::cleanup(const U& visitor) {
    for (std::uint32_t i = 0; i < cap; ++i) {
        for (unsigned int j = 0; j < cfg::Limits::MaxConcurrentFrames; ++j) {
            visitor(i, j, getRaw(i, j));
        }
    }
}

template<typename T>
T& PerFrameVector<T>::current(std::uint32_t i) {
    return items[i * cfg::Limits::MaxConcurrentFrames + vs->currentFrameIndex()];
}

template<typename T>
const T& PerFrameVector<T>::current(std::uint32_t i) const {
    return items[i * cfg::Limits::MaxConcurrentFrames + vs->currentFrameIndex()];
}

template<typename T>
T& PerFrameVector<T>::getRaw(std::uint32_t i, std::uint32_t frame) {
    return items[i * cfg::Limits::MaxConcurrentFrames + frame];
}

template<typename T>
const T& PerFrameVector<T>::getRaw(std::uint32_t i, std::uint32_t frame) const {
    return items[i * cfg::Limits::MaxConcurrentFrames + frame];
}

template<typename T>
constexpr std::uint32_t PerFrameVector<T>::size() const {
    return cap;
}

template<typename T>
constexpr std::uint32_t PerFrameVector<T>::totalSize() const {
    return items.size();
}

template<typename T>
constexpr T* PerFrameVector<T>::data() {
    return items.data();
}

template<typename T>
constexpr const T* PerFrameVector<T>::data() const {
    return items.data();
}

} // namespace vk
} // namespace rc
} // namespace bl

#endif
