#include <BLIB/Render/Vulkan/Swapchain.hpp>

#include <BLIB/Engine/Configuration.hpp>
#include <BLIB/Engine/Settings.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>
#include <Render/Vulkan/Utils/QueueFamilyLocator.hpp>
#include <Render/Vulkan/Utils/SwapChainSupportDetails.hpp>

namespace bl
{
namespace render
{
namespace vk
{
void Swapchain::Frame::init(VulkanState& vulkanState) {
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    if (vkCreateSemaphore(vulkanState.device, &semaphoreInfo, nullptr, &imageAvailableSemaphore) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to create semaphore");
    }
    if (vkCreateSemaphore(vulkanState.device, &semaphoreInfo, nullptr, &renderFinishedSemaphore) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to create semaphore");
    }

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    if (vkCreateFence(vulkanState.device, &fenceInfo, nullptr, &commandBufferFence) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create fence");
    }

    commandPool = vulkanState.createCommandPool(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT |
                                                VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    // create command buffer
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool        = commandPool;
    allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;
    if (vkAllocateCommandBuffers(vulkanState.device, &allocInfo, &commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate command buffers");
    }
}

void Swapchain::Frame::cleanup(VulkanState& vulkanState) {
    vkDestroySemaphore(vulkanState.device, imageAvailableSemaphore, nullptr);
    vkDestroySemaphore(vulkanState.device, renderFinishedSemaphore, nullptr);
    vkDestroyFence(vulkanState.device, commandBufferFence, nullptr);
    vkDestroyCommandPool(vulkanState.device, commandPool, nullptr);
}

Swapchain::Swapchain(VulkanState& state, sf::WindowBase& w)
: vulkanState(state)
, window(w)
, currentImageIndex(0)
, outOfDate(true) {}

void Swapchain::destroy() {
    cleanup();
    frameData.cleanup([this](Frame& frame) { frame.cleanup(vulkanState); });
}

void Swapchain::beginFrame(ColorAttachmentSet*& renderFrame, VkCommandBuffer& cb) {
    // wait for prior frame
    vkCheck(vkWaitForFences(
        vulkanState.device, 1, &frameData.current().commandBufferFence, VK_TRUE, UINT64_MAX));
    vkCheck(vkResetFences(vulkanState.device, 1, &frameData.current().commandBufferFence));

    // recreate if out of date
    if (outOfDate) { recreate(); }

    // acquire next image
    VkResult acquireResult;
    do {
        acquireResult = vkAcquireNextImageKHR(vulkanState.device,
                                              swapchain,
                                              UINT64_MAX,
                                              frameData.current().imageAvailableSemaphore,
                                              VK_NULL_HANDLE,
                                              &currentImageIndex);
        if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR) { recreate(); }
    } while (acquireResult == VK_ERROR_OUT_OF_DATE_KHR);
    if (acquireResult != VK_SUCCESS && acquireResult != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquire swapchain image");
    }

    // reset prior command buffer
    vkCheck(vkResetCommandBuffer(frameData.current().commandBuffer, 0));

    // begin command buffer
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    if (vkBeginCommandBuffer(frameData.current().commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin recording command buffer");
    }

    // copy output
    renderFrame = &renderFrames[currentImageIndex];
    cb          = frameData.current().commandBuffer;
}

void Swapchain::completeFrame() {
    // end command buffer
    vkCheck(vkEndCommandBuffer(frameData.current().commandBuffer));

    // submit to queue
    VkSemaphore waitSemaphores[]      = {frameData.current().imageAvailableSemaphore};
    VkSemaphore signalSemaphores[]    = {frameData.current().renderFinishedSemaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    VkSubmitInfo submitInfo{};
    submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount   = std::size(waitSemaphores);
    submitInfo.pWaitSemaphores      = waitSemaphores;
    submitInfo.pWaitDstStageMask    = waitStages;
    submitInfo.signalSemaphoreCount = std::size(signalSemaphores);
    submitInfo.pSignalSemaphores    = signalSemaphores;
    submitInfo.commandBufferCount   = 1;
    submitInfo.pCommandBuffers      = &frameData.current().commandBuffer;
    if (vkQueueSubmit(
            vulkanState.graphicsQueue, 1, &submitInfo, frameData.current().commandBufferFence) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to submit draw command buffer");
    }

    // trigger swap chain
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores    = &frameData.current().renderFinishedSemaphore;
    presentInfo.swapchainCount     = 1;
    presentInfo.pSwapchains        = &swapchain;
    presentInfo.pImageIndices      = &currentImageIndex;
    presentInfo.pResults           = nullptr; // Optional

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
    for (ColorAttachmentSet& frame : renderFrames) {
        vkDestroyImageView(vulkanState.device, frame.colorImageView(), nullptr);
    }
    vkDestroySwapchainKHR(vulkanState.device, swapchain, nullptr);
}

void Swapchain::create(VkSurfaceKHR s) {
    surface = s;
    frameData.init(vulkanState, [this](Frame& frame) { frame.init(vulkanState); });

    createSwapchain();
    createImageViews();
    outOfDate = false;
}

void Swapchain::recreate() {
    outOfDate = false;
    vkCheck(vkDeviceWaitIdle(vulkanState.device));

    cleanup();

    createSwapchain();
    createImageViews();
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
    createInfo.presentMode =
        swapChainSupport.presentationMode(!engine::Configuration::getOrDefault<bool>(
            engine::Settings::WindowParameters::VSyncKey, true));
    createInfo.clipped      = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    const VkResult result =
        vkCreateSwapchainKHR(vulkanState.device, &createInfo, nullptr, &swapchain);
    if (result != VK_SUCCESS) {
        BL_LOG_ERROR << "Swap chain creation failed: " << result;
        throw std::runtime_error("Failed to create swap chain");
    }

    // Fetch images
    VkImage images[8];
    vkCheck(vkGetSwapchainImagesKHR(vulkanState.device, swapchain, &imageCount, nullptr)); // count
    renderFrames.resize(imageCount);
    vkCheck(vkGetSwapchainImagesKHR(vulkanState.device, swapchain, &imageCount, images)); // fetch
    for (unsigned int i = 0; i < imageCount; ++i) {
        renderFrames[i].imageHandles[0] = images[i];
        renderFrames[i].extent          = createInfo.imageExtent;
    }
    imageFormat = surfaceFormat.format;
}

void Swapchain::createImageViews() {
    for (std::size_t i = 0; i < renderFrames.size(); i++) {
        renderFrames[i].imageViewHandles[0] =
            vulkanState.createImageView(renderFrames[i].colorImage(), imageFormat);
    }
}

} // namespace vk
} // namespace render
} // namespace bl
