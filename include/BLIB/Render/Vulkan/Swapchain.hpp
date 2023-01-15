#ifndef BLIB_RENDER_VULKAN_SWAPCHAIN_HPP
#define BLIB_RENDER_VULKAN_SWAPCHAIN_HPP

#include <BLIB/Render/Util/PerFrame.hpp>
#include <BLIB/Render/Util/PerSwapFrame.hpp>
#include <BLIB/Render/Vulkan/Framebuffer.hpp>
#include <SFML/Window/WindowBase.hpp>
#include <glad/vulkan.h>
#include <vector>

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

    constexpr std::uint32_t currentIndex() const;

    constexpr std::size_t length() const;

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
    // TODO - get rid of frame buffers here. Move to RenderPass. Create per-swap-frame util similar
    // to PerFrame to manage
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
