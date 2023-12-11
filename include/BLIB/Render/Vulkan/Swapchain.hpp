#ifndef BLIB_RENDER_VULKAN_SWAPCHAIN_HPP
#define BLIB_RENDER_VULKAN_SWAPCHAIN_HPP

#include <BLIB/Render/Vulkan/AttachmentBuffer.hpp>
#include <BLIB/Render/Vulkan/Framebuffer.hpp>
#include <BLIB/Render/Vulkan/PerFrame.hpp>
#include <BLIB/Render/Vulkan/PerSwapFrame.hpp>
#include <BLIB/Render/Vulkan/StandardAttachmentSet.hpp>
#include <BLIB/Vulkan.hpp>
#include <SFML/Window/WindowBase.hpp>
#include <vector>

namespace bl
{
namespace rc
{
namespace vk
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
     */
    void create();

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
    void beginFrame(StandardAttachmentSet*& renderFrame, VkCommandBuffer& commandBuffer);

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
    std::size_t length() const;

    /**
     * @brief Returns the StandardAttachmentSet of the swap chain at the given index
     *
     * @param i The index to get. UB if out of bounds
     * @return The swapchain image at the given index
     */
    const StandardAttachmentSet& swapFrameAtIndex(unsigned int i) const;

    /**
     * @brief Returns the image format of images in the swap chain
     *
     * @return The format of images in the swap chain
     */
    constexpr VkFormat swapImageFormat() const;

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

    VkSwapchainKHR swapchain;
    VkFormat imageFormat;
    std::vector<StandardAttachmentSet> renderFrames;
    std::vector<AttachmentBuffer> depthBuffers;
    vk::PerFrame<Frame> frameData;
    std::uint32_t currentImageIndex;
    bool outOfDate;

    void cleanup();
    void recreate();
    void createSwapchain();
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr std::uint32_t Swapchain::currentIndex() const { return currentImageIndex; }

inline std::size_t Swapchain::length() const { return renderFrames.size(); }

inline const StandardAttachmentSet& Swapchain::swapFrameAtIndex(unsigned int i) const {
    return renderFrames[i];
}

inline constexpr VkFormat Swapchain::swapImageFormat() const { return imageFormat; }

template<typename T>
PerSwapFrame<T>::PerSwapFrame()
: chain(nullptr) {}

template<typename T>
template<typename TCb>
void PerSwapFrame<T>::init(Swapchain& c, const TCb& visitor) {
    chain = &c;
    data.clear();
    data.reserve(chain->length());
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

} // namespace vk
} // namespace rc
} // namespace bl

#endif
