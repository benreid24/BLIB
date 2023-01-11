#include <BLIB/Render/Vulkan/Swapchain.hpp>

#include <BLIB/Render/Vulkan/VulkanState.hpp>
#include <Render/Vulkan/Utils/QueueFamilyLocator.hpp>
#include <Render/Vulkan/Utils/SwapChainSupportDetails.hpp>

namespace bl
{
namespace render
{
Swapchain::Swapchain(VulkanState& state, sf::WindowBase& w)
: vulkanState(state)
, window(w)
, outOfDate(true) {
    const auto createSem = [this](VkSemaphore& sem) {
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        if (vkCreateSemaphore(vulkanState.device, &semaphoreInfo, nullptr, &sem) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create semaphore");
        }
    };

    imageAvailableSemaphores.init(vulkanState, createSem);
    frameFinishedSemaphores.init(vulkanState, createSem);
    frameFences.init(vulkanState, [this](VkFence& fence) {
        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        if (vkCreateFence(vulkanState.device, &fenceInfo, nullptr, &fence) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create fence");
        }
    });
}

void Swapchain::destroy() {
    cleanup();

    const auto destroySem = [this](VkSemaphore sem) {
        vkDestroySemaphore(vulkanState.device, sem, nullptr);
    };
    imageAvailableSemaphores.cleanup(destroySem);
    frameFinishedSemaphores.cleanup(destroySem);
    frameFences.cleanup(
        [this](VkFence fence) { vkDestroyFence(vulkanState.device, fence, nullptr); });
}

VkCommandBuffer Swapchain::beginFrame() {
    // wait for prior frame
    vkWaitForFences(vulkanState.device, 1, &frameFences.current(), VK_TRUE, UINT64_MAX);
    vkResetFences(vulkanState.device, 1, &frameFences.current());

    // recreate if out of date
    if (outOfDate) { recreate(); }

    // acquire next image
    VkResult acquireResult;
    do {
        acquireResult = vkAcquireNextImageKHR(vulkanState.device,
                                              swapchain,
                                              UINT64_MAX,
                                              imageAvailableSemaphores.current(),
                                              VK_NULL_HANDLE,
                                              &currentImageIndex);
        if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR) { recreate(); }
    } while (acquireResult == VK_ERROR_OUT_OF_DATE_KHR);
    if (acquireResult != VK_SUCCESS && acquireResult != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquire swapchain image");
    }

    return framebuffers[currentImageIndex].beginRender();
}

void Swapchain::completeFrame() {
    // submit render commands
    framebuffers[currentImageIndex].finishRender(imageAvailableSemaphores.current(),
                                                 frameFinishedSemaphores.current(),
                                                 frameFences.current());

    // trigger swap chain
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores    = &frameFinishedSemaphores.current();
    presentInfo.swapchainCount  = 1;
    presentInfo.pSwapchains     = &swapchain;
    presentInfo.pImageIndices   = &currentImageIndex;
    presentInfo.pResults        = nullptr; // Optional

    const VkResult presentResult = vkQueuePresentKHR(vulkanState.presentQueue, &presentInfo);
    switch (presentResult) {
    case VK_ERROR_OUT_OF_DATE_KHR:
    case VK_SUBOPTIMAL_KHR:
        recreate();
        break;
    case VK_SUCCESS:
        break;
    default:
        throw std::runtime_error("Failed to present swap chain image");
    }
}

void Swapchain::cleanup() {
    framebuffers.clear();
    for (VkImageView imageView : imageViews) {
        vkDestroyImageView(vulkanState.device, imageView, nullptr);
    }
    vkDestroySwapchainKHR(vulkanState.device, swapchain, nullptr);
}

void Swapchain::create(VkSurfaceKHR s, VkRenderPass r) {
    surface    = s;
    renderPass = r;

    createSwapchain();
    createImageViews();
    createFramebuffers();
}

void Swapchain::recreate() {
    outOfDate = false;
    vkDeviceWaitIdle(vulkanState.device);

    cleanup();

    createSwapchain();
    createImageViews();
    createFramebuffers();
}

void Swapchain::invalidate() { outOfDate = true; }

void Swapchain::createSwapchain() {
    // get supported swap chain details
    SwapChainSupportDetails swapChainSupport;
    swapChainSupport.populate(vulkanState.physicalDevice, surface);
    const VkSurfaceFormatKHR& surfaceFormat = swapChainSupport.swapSurfaceFormat();

    // image count in swap chain
    std::uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 &&
        imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    // swap chain create params
    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface          = surface;
    createInfo.minImageCount    = imageCount;
    createInfo.imageFormat      = surfaceFormat.format;
    createInfo.imageColorSpace  = surfaceFormat.colorSpace;
    createInfo.imageExtent      = swapChainSupport.swapExtent(window.getSize());
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    // queue chain config
    QueueFamilyLocator indices;
    indices.populate(vulkanState.physicalDevice, surface);
    std::uint32_t queueFamilies[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = std::size(queueFamilies);
        createInfo.pQueueFamilyIndices   = queueFamilies;
    }
    else {
        createInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;       // Optional
        createInfo.pQueueFamilyIndices   = nullptr; // Optional
    }

    // more create params
    createInfo.preTransform   = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // for other windows/applications
    createInfo.presentMode    = swapChainSupport.presentationMode();
    createInfo.clipped        = VK_TRUE;
    createInfo.oldSwapchain   = VK_NULL_HANDLE;

    const VkResult result =
        vkCreateSwapchainKHR(vulkanState.device, &createInfo, nullptr, &swapchain);
    if (result != VK_SUCCESS) {
        BL_LOG_ERROR << "Swap chain creation failed: " << result;
        throw std::runtime_error("Failed to create swap chain");
    }

    vkGetSwapchainImagesKHR(vulkanState.device, swapchain, &imageCount, nullptr);
    images.resize(imageCount);
    vkGetSwapchainImagesKHR(vulkanState.device, swapchain, &imageCount, images.data());

    imageFormat = surfaceFormat.format;
    extent      = createInfo.imageExtent;
}

void Swapchain::createImageViews() {
    imageViews.resize(imageViews.size());

    for (std::size_t i = 0; i < images.size(); i++) {
        imageViews[i] = vulkanState.createImageView(images[i], imageFormat);
    }
}

void Swapchain::createFramebuffers() {
    framebuffers.reserve(imageViews.size());
    for (unsigned int i = 0; i < imageViews.size(); ++i) {
        VkImageView attachments[] = {imageViews[i]};
        framebuffers.emplace_back(vulkanState);
        framebuffers.back().create(renderPass, extent, attachments, std::size(attachments));
    }
}

} // namespace render
} // namespace bl
