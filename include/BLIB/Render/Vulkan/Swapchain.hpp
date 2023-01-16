#ifndef BLIB_RENDER_VULKAN_SWAPCHAIN_HPP
#define BLIB_RENDER_VULKAN_SWAPCHAIN_HPP

#include <BLIB/Render/Util/PerFrame.hpp>
#include <BLIB/Render/Util/PerSwapFrame.hpp>
#include <BLIB/Render/Vulkan/Framebuffer.hpp>
#include <BLIB/Render/Vulkan/SwapRenderFrame.hpp>
#include <SFML/Window/WindowBase.hpp>
#include <glad/vulkan.h>
#include <vector>

namespace bl
{
namespace render
{
struct VulkanState;

/**
 * @brief Utility class for managing the Vulkan swap chain
 *
 * @ingroup Renderer
 */
class Swapchain {
public:
    /**
     * @brief Creates a new swap chain
     *
     * @param vulkanState The renderer vulkan state
     * @param window The window to attach to
     */
    Swapchain(VulkanState& vulkanState, sf::WindowBase& window);

    /**
     * @brief Destroys the swap chain and other held resources
     */
    void destroy();

    /**
     * @brief Creates the swap chain itself
     * @param surface The surface to present to
     */
    void create(VkSurfaceKHR surface);

    /**
     * @brief Invalidates the swap chain. Will cause the swapchain to be recreated on the next
     *        rendering
     */
    void invalidate();

    /**
     * @brief Begins the rendering of a new frame. May block if the prior submitted frame is not yet
     *        done being read by the GPU
     *
     * @param renderFrame A reference to a pointer to populate with the active chain image
     * @param commandBuffer A command buffer reference to populate with the primary CB to use
     */
    void beginFrame(SwapRenderFrame*& renderFrame, VkCommandBuffer& commandBuffer);

    /**
     * @brief Finalizes the command buffer and submits it. Also triggers swap chain presentation
     */
    void completeFrame();

    /**
     * @brief Returns the current swap chain image index
     * 
     * @return The current swap chain image index
    */
    constexpr std::uint32_t currentIndex() const;

    /**
     * @brief Returns the number of images in the swap chain
     * 
     * @return The number of images in the swap chain
    */
    constexpr std::size_t length() const;

private:
    struct Frame {
        VkSemaphore imageAvailableSemaphore;
        VkSemaphore renderFinishedSemaphore;
        VkFence commandBufferFence;
        VkCommandPool commandPool;
        VkCommandBuffer commandBuffer;

        void init(VulkanState& vulkanState);
        void cleanup(VulkanState& vulkanState);
    };

    VulkanState& vulkanState;
    sf::WindowBase& window;
    VkSurfaceKHR surface;

    VkSwapchainKHR swapchain;
    VkFormat imageFormat;
    std::vector<SwapRenderFrame> renderFrames;
    PerFrame<Frame> frameData;
    std::uint32_t currentImageIndex;
    bool outOfDate;

    void cleanup();
    void recreate();
    void createSwapchain();
    void createImageViews();
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr std::uint32_t Swapchain::currentIndex() const { return currentImageIndex; }

template<typename T>
PerSwapFrame<T>::PerSwapFrame()
: chain(nullptr) {}

template<typename T>
template<typename TCb>
void PerSwapFrame<T>::init(Swapchain& c, const TCb& visitor) {
    chain = &c;
    data.clear();
    data.reserve(chain.length());
    for (unsigned int i = 0; i < chain->length(); ++i) {
        data.emplace_back();
        visitor(data.back());
    }
}

template<typename T>
template<typename TCb>
void PerSwapFrame<T>::cleanup(const TCb& visitor) {
    for (T& d : data) { visitor(d); }
}

template<typename T>
constexpr T& PerSwapFrame<T>::current() {
    return data[chain->currentIndex()];
}

template<typename T>
constexpr const T& PerSwapFrame<T>::current() const {
    return data[chain->currentIndex()];
}

} // namespace render
} // namespace bl

#endif
