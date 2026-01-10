#include <BLIB/Render/Vulkan/Swapchain.hpp>

#include <BLIB/Engine/Configuration.hpp>
#include <BLIB/Engine/Settings.hpp>
#include <BLIB/Render/Renderer.hpp>
#include <BLIB/Render/Vulkan/VulkanLayer.hpp>
#include <Render/Vulkan/Utils/QueueFamilyLocator.hpp>
#include <Render/Vulkan/Utils/SwapChainSupportDetails.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
void Swapchain::Frame::init(VulkanLayer& vulkanState) {
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    if (vkCreateSemaphore(
            vulkanState.getDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphore) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to create semaphore");
    }

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    if (vkCreateFence(vulkanState.getDevice(), &fenceInfo, nullptr, &commandBufferFence) !=
        VK_SUCCESS) {
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
    if (vkAllocateCommandBuffers(vulkanState.getDevice(), &allocInfo, &commandBuffer) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate command buffers");
    }
}

void Swapchain::Frame::cleanup(VulkanLayer& vulkanState) {
    vkDestroySemaphore(vulkanState.getDevice(), renderFinishedSemaphore, nullptr);
    vkDestroyFence(vulkanState.getDevice(), commandBufferFence, nullptr);
    vkDestroyCommandPool(vulkanState.getDevice(), commandPool, nullptr);
}

void Swapchain::Swapframes::init(VulkanLayer& vulkanState) {
    currentIndex = 0;

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    for (VkSemaphore& sem : imageAvailableSemaphore) {
        if (vkCreateSemaphore(vulkanState.getDevice(), &semaphoreInfo, nullptr, &sem) !=
            VK_SUCCESS) {
            throw std::runtime_error("Failed to create semaphore");
        }
    }
}

void Swapchain::Swapframes::cleanup(VulkanLayer& vulkanState) {
    for (VkSemaphore sem : imageAvailableSemaphore) {
        vkDestroySemaphore(vulkanState.getDevice(), sem, nullptr);
    }
}

VkSemaphore Swapchain::Swapframes::getNext() {
    currentIndex = (currentIndex + 1) % imageAvailableSemaphore.size();
    return imageAvailableSemaphore[currentIndex];
}

VkSemaphore Swapchain::Swapframes::current() { return imageAvailableSemaphore[currentIndex]; }

Swapchain::Swapchain(Renderer& renderer, sf::WindowBase& w, WindowSettings& ws)
: renderer(renderer)
, window(w)
, windowSettings(ws)
, swapchain(VK_NULL_HANDLE)
, oldSwapchain(VK_NULL_HANDLE)
, oldSurface(VK_NULL_HANDLE)
, currentImageIndex(0)
, outOfDate(true) {}

void Swapchain::destroy() {
    cleanup();
    frameData.cleanup([this](Frame& frame) { frame.cleanup(renderer.vulkanState()); });
    imageSemaphores.cleanup(renderer.vulkanState());
}

void Swapchain::beginFrame(AttachmentSet*& renderFrame, VkCommandBuffer& cb) {
    // wait for prior frame
    vkCheck(vkWaitForFences(renderer.vulkanState().getDevice(),
                            1,
                            &frameData.current().commandBufferFence,
                            VK_TRUE,
                            UINT64_MAX));
    vkCheck(vkResetFences(
        renderer.vulkanState().getDevice(), 1, &frameData.current().commandBufferFence));

    // recreate if out of date
    if (outOfDate) { recreate(); }

    // acquire next image
    VkResult acquireResult;
    do {
        acquireResult = vkAcquireNextImageKHR(renderer.vulkanState().getDevice(),
                                              swapchain,
                                              UINT64_MAX,
                                              imageSemaphores.getNext(),
                                              VK_NULL_HANDLE,
                                              &currentImageIndex);
        if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR) { recreate(); }
    } while (acquireResult == VK_ERROR_OUT_OF_DATE_KHR);
    if (acquireResult != VK_SUCCESS && acquireResult != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquire swapchain image");
    }

    // reset prior command buffer & fence
    vkCheck(vkResetCommandBuffer(frameData.current().commandBuffer, 0));
    vkCheck(vkResetFences(
        renderer.vulkanState().getDevice(), 1, &frameData.current().commandBufferFence));

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
    VkSemaphore waitSemaphores[]      = {imageSemaphores.current()};
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
    renderer.vulkanState().submitCommandBuffer(submitInfo, frameData.current().commandBufferFence);

    // trigger swap chain
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores    = &frameData.current().renderFinishedSemaphore;
    presentInfo.swapchainCount     = 1;
    presentInfo.pSwapchains        = &swapchain;
    presentInfo.pImageIndices      = &currentImageIndex;
    presentInfo.pResults           = nullptr; // Optional

    const VkResult presentResult =
        vkQueuePresentKHR(renderer.vulkanState().getPresentQueue(), &presentInfo);
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
    for (AttachmentSet& frame : renderFrames) {
        vkDestroyImageView(renderer.vulkanState().getDevice(), frame.getImageView(0), nullptr);
    }
    vkDestroySwapchainKHR(renderer.vulkanState().getDevice(), swapchain, nullptr);
}

void Swapchain::deferCleanup() {
    for (AttachmentSet& frame : renderFrames) {
        renderer.getCleanupManager().add(
            [device = renderer.vulkanState().getDevice(), view = frame.getImageView(0)]() {
                vkDestroyImageView(device, view, nullptr);
            });
    }
    renderer.getCleanupManager().add(
        [device = renderer.vulkanState().getDevice(), chain = swapchain]() {
            vkDestroySwapchainKHR(device, chain, nullptr);
        });
}

void Swapchain::create() {
    createSwapchain();
    frameData.init(*this, [this](Frame& frame) { frame.init(renderer.vulkanState()); });
    imageSemaphores.init(renderer.vulkanState());
    outOfDate = false;
}

void Swapchain::recreate() {
    outOfDate = false;
    if (renderer.vulkanState().getSurface() == oldSurface) {
        oldSwapchain = swapchain;
        deferCleanup();
        vkCheck(vkDeviceWaitIdle(renderer.vulkanState().getDevice()));
    }
    else {
        vkCheck(vkDeviceWaitIdle(renderer.vulkanState().getDevice()));
        cleanup();
        oldSwapchain = VK_NULL_HANDLE;
    }
    createSwapchain();
}

void Swapchain::invalidate() { outOfDate = true; }

void Swapchain::createSwapchain() {
    // cache the surface that we created with
    oldSurface = renderer.vulkanState().getSurface();

    // get supported swap chain details
    SwapChainSupportDetails swapChainSupport;
    swapChainSupport.populate(renderer.vulkanState().getPhysicalDevice(),
                              renderer.vulkanState().getSurface());
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
    createInfo.surface          = renderer.vulkanState().getSurface();
    createInfo.minImageCount    = imageCount;
    createInfo.imageFormat      = surfaceFormat.format;
    createInfo.imageColorSpace  = surfaceFormat.colorSpace;
    createInfo.imageExtent      = swapChainSupport.swapExtent(window.getSize());
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

    // queue chain config
    QueueFamilyLocator indices;
    indices.populate(renderer.vulkanState().getPhysicalDevice(),
                     renderer.vulkanState().getSurface());
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
    createInfo.presentMode    = swapChainSupport.presentationMode(!windowSettings.vsyncEnabled());
    createInfo.clipped        = VK_TRUE;
    createInfo.oldSwapchain   = oldSwapchain;

    const VkResult result =
        vkCreateSwapchainKHR(renderer.vulkanState().getDevice(), &createInfo, nullptr, &swapchain);
    if (result != VK_SUCCESS) {
        BL_LOG_ERROR << "Swap chain creation failed: " << result;
        throw std::runtime_error("Failed to create swap chain");
    }

    // Fetch images
    VkImage images[8];
    vkCheck(vkGetSwapchainImagesKHR(
        renderer.vulkanState().getDevice(), swapchain, &imageCount, images));
    imageFormat = surfaceFormat.format;

    // assign attachment sets & transition images
    auto cb = renderer.getSharedCommandPool().createBuffer();
    renderFrames.clear();
    renderFrames.reserve(imageCount);
    std::array<VkImageAspectFlags, 1> aspects{VK_IMAGE_ASPECT_COLOR_BIT};
    for (unsigned int i = 0; i < imageCount; ++i) {
        VkImageView view = renderer.vulkanState().createImageView(images[i], imageFormat);
        renderFrames.emplace_back(1, &images[i], &view, aspects.data(), createInfo.imageExtent);
        renderer.vulkanState().transitionImageLayout(cb,
                                                     images[i],
                                                     VK_IMAGE_LAYOUT_UNDEFINED,
                                                     VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                                     1,
                                                     VK_IMAGE_ASPECT_COLOR_BIT);
    }
    cb.submit();
}

} // namespace vk
} // namespace rc
} // namespace bl
