#include "RendererState.hpp"

#include "QueueFamilyLocator.hpp"
#include "SwapChainSupportDetails.hpp"
#include "TransformUniform.hpp"
#include <BLIB/Logging.hpp>
#include <set>
#include <string>
#include <unordered_set>

namespace
{
#ifdef BLIB_DEBUG
const std::unordered_set<std::string> RequestedValidationLayers{"VK_LAYER_KHRONOS_validation",
                                                                "VK_LAYER_LUNARG_monitor"};
#endif

const std::vector<const char*> RequiredDeviceExtensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};

#ifdef BLIB_DEBUG
VKAPI_ATTR VkBool32 VKAPI_CALL
debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
              VkDebugUtilsMessageTypeFlagsEXT messageType,
              const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void*) {
    const char* mtypeStr = "Unknown";
    switch (messageType) {
    case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
        mtypeStr = "General";
        break;
    case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
        mtypeStr = "Validation";
        break;
    case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
        mtypeStr = "Performance";
        break;
    default:
        break;
    }

    switch (messageSeverity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        BL_LOG_TRACE << mtypeStr << ": " << pCallbackData->pMessage;
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        BL_LOG_INFO << mtypeStr << ": " << pCallbackData->pMessage;
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        BL_LOG_WARN << mtypeStr << ": " << pCallbackData->pMessage;
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        BL_LOG_ERROR << mtypeStr << ": " << pCallbackData->pMessage;
        break;
    default:
        BL_LOG_DEBUG << mtypeStr << ": " << pCallbackData->pMessage;
        break;
    }

    return VK_FALSE;
}
#endif

bool deviceHasRequiredExtensions(VkPhysicalDevice device, const char* name) {
    std::uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(
        device, nullptr, &extensionCount, availableExtensions.data());

    std::unordered_set<std::string> extensionSet;
    for (const VkExtensionProperties& ext : availableExtensions) {
        extensionSet.emplace(ext.extensionName);
    }

    for (const char* ext : RequiredDeviceExtensions) {
        if (extensionSet.find(ext) == extensionSet.end()) {
            BL_LOG_WARN << name << " is missing required extension " << ext;
            return false;
        }
    }

    return true;
}

int scorePhysicalDevice(VkPhysicalDevice device, const VkPhysicalDeviceProperties& deviceProperties,
                        const VkPhysicalDeviceFeatures& deviceFeatures,
                        const QueueFamilyLocator& queueFamilies, VkSurfaceKHR surface) {
    // requirements
    if (!deviceFeatures.geometryShader) return -1;
    if (!deviceFeatures.samplerAnisotropy) return -1;
    if (!queueFamilies.complete()) return -1;
    if (!deviceHasRequiredExtensions(device, deviceProperties.deviceName)) return -1;
    SwapChainSupportDetails swapChainDetails(device, surface);
    if (swapChainDetails.formats.empty() || swapChainDetails.presentModes.empty()) return -1;

    // preferred optional features
    int score = 0;
    score += deviceProperties.limits.maxImageDimension2D;
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) { score += 5000; }

    return score;
}
} // namespace

RendererState::RendererState(sf::WindowBase& window)
: window(window)
, currentFrame(0)
, swapChainOutOfDate(false) {
    gladLoadVulkan(0, sf::Vulkan::getFunction);
    if (!vkCreateInstance) { throw std::runtime_error("Failed to load Vulkan"); }

    createInstance();
#ifdef BLIB_DEBUG
    setupDebugMessenger();
#endif
    createSurface();
    pickPhysicalDevice();
    gladLoadVulkan(physicalDevice, sf::Vulkan::getFunction); // reload with extensions
    createLogicalDevice();
    createSwapChain();
    createImageViews();
    createCommandPool();
    createRenderFrames();
    createDescriptorSetLayout();
    createUniformBuffers();
    createDescriptorPool();
    createDescriptorSets();
}

RendererState::~RendererState() {
    for (unsigned int i = 0; i < uniformBuffers.size(); ++i) {
        vkDestroyBuffer(device, uniformBuffers[i], nullptr);
        vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
    }
    vkDestroyDescriptorPool(device, descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
    vkFreeCommandBuffers(device, commandPool, 1, &transferCommandBuffer);
    cleanupSwapchain();
    for (RenderSwapFrame& frame : renderFrames) { frame.cleanup(device); }
    vkDestroyCommandPool(device, commandPool, nullptr);
    vkDestroyDevice(device, nullptr);
#ifdef BLIB_DEBUG
    cleanupDebugMessenger();
#endif
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
}

void RendererState::finalizeInitialization(VkRenderPass rp) {
    renderPass = rp;
    createFramebuffers();
}

void RendererState::invalidateSwapChain() { swapChainOutOfDate = true; }

RenderSwapFrame* RendererState::beginFrame(VkRenderPassBeginInfo& renderPassInfo,
                                           VkDescriptorSet& descriptorSet) {
    RenderSwapFrame& frame = renderFrames[currentFrame];
    descriptorSet          = descriptorSets[currentFrame];

    // wait for previous frame
    vkWaitForFences(device, 1, &frame.inFlightFence, VK_TRUE, UINT64_MAX);

    // get image to render to
    const VkResult acquireResult = vkAcquireNextImageKHR(device,
                                                         swapChain,
                                                         UINT64_MAX,
                                                         frame.imageAvailableSemaphore,
                                                         VK_NULL_HANDLE,
                                                         &currentRenderImageIndex);
    switch (acquireResult) {
    case VK_ERROR_OUT_OF_DATE_KHR:
        recreateSwapChain();
        return nullptr;
    case VK_SUBOPTIMAL_KHR:
    case VK_SUCCESS:
        break;
    default:
        throw std::runtime_error("Failed to acquire swap chain image");
    }

    // reset fence only once we know we are submitting render commands
    vkResetFences(device, 1, &frame.inFlightFence);

    // start recording render commands
    vkResetCommandBuffer(frame.commandBuffer, 0);
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags            = 0;       // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional
    if (vkBeginCommandBuffer(frame.commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin recording command buffer");
    }

    // populate render pass begin info with what we have internally
    renderPassInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass        = renderPass;
    renderPassInfo.framebuffer       = swapChainFramebuffers[currentRenderImageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapChainExtent;

    return &frame;
}

void RendererState::completeFrame() {
    RenderSwapFrame& frame = renderFrames[currentFrame];

    // submit command buffer
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[]      = {frame.imageAvailableSemaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount     = std::size(waitSemaphores);
    submitInfo.pWaitSemaphores        = waitSemaphores;
    submitInfo.pWaitDstStageMask      = waitStages;
    submitInfo.commandBufferCount     = 1;
    submitInfo.pCommandBuffers        = &frame.commandBuffer;

    VkSemaphore signalSemaphores[]  = {frame.renderFinishedSemaphore};
    submitInfo.signalSemaphoreCount = std::size(signalSemaphores);
    submitInfo.pSignalSemaphores    = signalSemaphores;

    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, frame.inFlightFence) != VK_SUCCESS) {
        throw std::runtime_error("Failed to submit draw command buffer");
    }

    // trigger swap chain
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores    = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapChain};
    presentInfo.swapchainCount  = std::size(swapChains);
    presentInfo.pSwapchains     = swapChains;
    presentInfo.pImageIndices   = &currentRenderImageIndex;
    presentInfo.pResults        = nullptr; // Optional

    const VkResult presentResult = vkQueuePresentKHR(presentQueue, &presentInfo);
    switch (presentResult) {
    case VK_ERROR_OUT_OF_DATE_KHR:
    case VK_SUBOPTIMAL_KHR:
        recreateSwapChain();
        break;
    case VK_SUCCESS:
        break;
    default:
        throw std::runtime_error("Failed to present swap chain image");
    }
    if (swapChainOutOfDate) { recreateSwapChain(); }

    currentFrame = (currentFrame + 1) % MaxConcurrentFrames;
}

void RendererState::createInstance() {
    // app info
    VkApplicationInfo appInfo{};
    appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName   = "Vulkan Sandbox";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName        = "BLIB";
    appInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion         = VK_API_VERSION_1_2;

    // instance data to populate
    VkInstanceCreateInfo createInfo{};
    createInfo.sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

// validation layers
#ifdef BLIB_DEBUG
    std::vector<VkLayerProperties> validLayers;
    std::vector<const char*> validationLayers;
    std::uint32_t layerCount = 0;
    if (vkEnumerateInstanceLayerProperties(&layerCount, 0) != VK_SUCCESS) {
        throw std::runtime_error("Failed to enumerate validation layers");
    }
    validLayers.resize(layerCount);
    if (vkEnumerateInstanceLayerProperties(&layerCount, validLayers.data()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to retrieve validation layers");
    }
    validationLayers.reserve(RequestedValidationLayers.size());
    for (const VkLayerProperties& layer : validLayers) {
        // BL_LOG_DEBUG << "Found validation layer: " << layer.layerName;
        if (RequestedValidationLayers.find(layer.layerName) != RequestedValidationLayers.end()) {
            validationLayers.emplace_back(layer.layerName);
        }
    }
    createInfo.enabledLayerCount   = validationLayers.size();
    createInfo.ppEnabledLayerNames = validationLayers.data();
#else
    createInfo.enabledLayerCount = 0;
#endif

    // query valid extensions
    std::uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
    std::unordered_set<std::string> extensionsSet;
    for (const VkExtensionProperties& ext : extensions) {
        extensionsSet.emplace(ext.extensionName);
    }

    // required extensions
    std::vector<const char*> requiredExtentions =
        sf::Vulkan::getGraphicsRequiredInstanceExtensions();
#ifdef BLIB_DEBUG
    requiredExtentions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    requiredExtentions.emplace_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif
    for (const char* ext : requiredExtentions) {
        if (extensionsSet.find(ext) == extensionsSet.end()) {
            BL_LOG_ERROR << "Missing required extension: " << ext;
            throw std::runtime_error("Missing required extension");
        }
    }
    createInfo.enabledExtensionCount   = requiredExtentions.size();
    createInfo.ppEnabledExtensionNames = requiredExtentions.data();

    // actual instance creation
    const VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
    if (result != VK_SUCCESS) {
        BL_LOG_ERROR << "Got instance creation error code: " << result;
        throw std::runtime_error("Failed to create Vulkan instance");
    }
}

#ifdef BLIB_DEBUG
void RendererState::setupDebugMessenger() {
    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    createInfo.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;

    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        if (func(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create debug messenger");
        }
    }
    else { throw std::runtime_error("Failed to load vkCreateDebugUtilsMessengerEXT"); }
}
#endif

void RendererState::createSurface() {
    if (!window.createVulkanSurface(instance, surface)) {
        throw std::runtime_error("Failed to create Vulkcan surface");
    }
}

void RendererState::pickPhysicalDevice() {
    std::uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0) { throw std::runtime_error("Failed to find GPUs with Vulkan support"); }
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    int pscore = -1;
    for (const auto& device : devices) {
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        QueueFamilyLocator queueFamilies;
        queueFamilies.populate(device, surface);

        const int s =
            scorePhysicalDevice(device, deviceProperties, deviceFeatures, queueFamilies, surface);
        if (s >= pscore) {
            physicalDevice = device;
            pscore         = s;
        }
    }

    if (pscore < 0 || physicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("Failed to find a suitable GPU");
    }
}

void RendererState::createLogicalDevice() {
    QueueFamilyLocator indices;
    indices.populate(physicalDevice, surface);

    // queue(s) info
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<std::uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(),
                                                   indices.presentFamily.value()};
    const float queuePriorities[]               = {1.f};
    queueCreateInfos.reserve(uniqueQueueFamilies.size());
    for (std::uint32_t queueIndex : uniqueQueueFamilies) {
        queueCreateInfos.emplace_back();
        VkDeviceQueueCreateInfo& queueCreateInfo = queueCreateInfos.back();
        queueCreateInfo.sType                    = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex         = queueIndex;
        queueCreateInfo.queueCount               = 1;
        queueCreateInfo.pQueuePriorities         = queuePriorities;
    }

    // required features
    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    // logical device creation
    VkDeviceCreateInfo createInfo{};
    createInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos       = queueCreateInfos.data();
    createInfo.queueCreateInfoCount    = queueCreateInfos.size();
    createInfo.pEnabledFeatures        = &deviceFeatures;
    createInfo.enabledExtensionCount   = RequiredDeviceExtensions.size();
    createInfo.ppEnabledExtensionNames = RequiredDeviceExtensions.data();
    createInfo.enabledLayerCount       = 0; // never used

    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create logical device");
    }

    vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
}

void RendererState::createSwapChain() {
    // get supported swap chain details
    SwapChainSupportDetails swapChainSupport;
    swapChainSupport.populate(physicalDevice, surface);
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
    indices.populate(physicalDevice, surface);
    std::uint32_t QueueFamilyLocator[] = {indices.graphicsFamily.value(),
                                          indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = std::size(QueueFamilyLocator);
        createInfo.pQueueFamilyIndices   = QueueFamilyLocator;
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

    const VkResult result = vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain);
    if (result != VK_SUCCESS) {
        BL_LOG_ERROR << "Swap chain creation failed: " << result;
        throw std::runtime_error("Failed to create swap chain");
    }

    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent      = createInfo.imageExtent;
}

void RendererState::createImageViews() {
    swapChainImageViews.resize(swapChainImages.size());

    for (std::size_t i = 0; i < swapChainImages.size(); i++) {
        swapChainImageViews[i] = createImageView(swapChainImages[i], swapChainImageFormat);
    }
}

void RendererState::createCommandPool() {
    QueueFamilyLocator QueueFamilyLocator;
    QueueFamilyLocator.populate(physicalDevice, surface);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = QueueFamilyLocator.graphicsFamily.value();

    if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create command pool");
    }

    // create long-lived command-buffer for memory transfers
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool        = commandPool;
    allocInfo.commandBufferCount = 1;
    vkAllocateCommandBuffers(device, &allocInfo, &transferCommandBuffer);
}

void RendererState::createRenderFrames() {
    for (RenderSwapFrame& frame : renderFrames) { frame.initialize(device, commandPool); }
}

void RendererState::createFramebuffers() {
    swapChainFramebuffers.resize(swapChainImageViews.size());
    for (std::size_t i = 0; i < swapChainImageViews.size(); i++) {
        VkImageView attachments[] = {swapChainImageViews[i]};

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass      = renderPass;
        framebufferInfo.attachmentCount = std::size(attachments);
        framebufferInfo.pAttachments    = attachments;
        framebufferInfo.width           = swapChainExtent.width;
        framebufferInfo.height          = swapChainExtent.height;
        framebufferInfo.layers          = 1;

        if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) !=
            VK_SUCCESS) {
            throw std::runtime_error("Failed to create framebuffer");
        }
    }
}

void RendererState::createDescriptorSetLayout() {
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding            = 0;
    uboLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount    = 1;
    uboLayoutBinding.stageFlags         = VK_SHADER_STAGE_VERTEX_BIT;
    uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding            = 1;
    samplerLayoutBinding.descriptorCount    = 1;
    samplerLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;

    const std::array<VkDescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding,
                                                                  samplerLayoutBinding};
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<std::uint32_t>(bindings.size());
    layoutInfo.pBindings    = bindings.data();
    if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor set layout");
    }
}

void RendererState::createUniformBuffers() {
    VkDeviceSize bufferSize = sizeof(TransformUniform);

    for (size_t i = 0; i < MaxConcurrentFrames; i++) {
        createBuffer(bufferSize,
                     VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                     uniformBuffers[i],
                     uniformBuffersMemory[i]);

        vkMapMemory(device, uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
    }
}

void RendererState::createDescriptorPool() {
    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(MaxConcurrentFrames);
    poolSizes[1].type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(MaxConcurrentFrames);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = poolSizes.size();
    poolInfo.pPoolSizes    = poolSizes.data();
    poolInfo.maxSets       = static_cast<std::uint32_t>(MaxConcurrentFrames);

    if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor pool");
    }
}

void RendererState::createDescriptorSets() {
    std::array<VkDescriptorSetLayout, MaxConcurrentFrames> layouts{descriptorSetLayout,
                                                                   descriptorSetLayout};
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool     = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<std::uint32_t>(MaxConcurrentFrames);
    allocInfo.pSetLayouts        = layouts.data();

    if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate descriptor sets");
    }

    for (unsigned int i = 0; i < descriptorSets.size(); ++i) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range  = sizeof(TransformUniform);

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet           = descriptorSets[i];
        descriptorWrite.dstBinding       = 0;
        descriptorWrite.dstArrayElement  = 0;
        descriptorWrite.descriptorType   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount  = 1;
        descriptorWrite.pBufferInfo      = &bufferInfo;
        descriptorWrite.pImageInfo       = nullptr; // Optional
        descriptorWrite.pTexelBufferView = nullptr; // Optional

        // TODO - would need to write texture uniform here as well for example to work

        vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
    }
}

void RendererState::recreateSwapChain() {
    swapChainOutOfDate = false;
    vkDeviceWaitIdle(device);

    cleanupSwapchain();

    createSwapChain();
    createImageViews();
    createFramebuffers();
}

void RendererState::cleanupSwapchain() {
    for (VkFramebuffer framebuffer : swapChainFramebuffers) {
        vkDestroyFramebuffer(device, framebuffer, nullptr);
    }
    for (VkImageView imageView : swapChainImageViews) {
        vkDestroyImageView(device, imageView, nullptr);
    }
    vkDestroySwapchainKHR(device, swapChain, nullptr);
}

#ifdef BLIB_DEBUG
void RendererState::cleanupDebugMessenger() {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) { func(instance, debugMessenger, nullptr); }
}
#endif

std::uint32_t RendererState::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
    for (std::uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

void RendererState::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                                 VkMemoryPropertyFlags properties, VkBuffer& buffer,
                                 VkDeviceMemory& bufferMemory) {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size        = size;
    bufferInfo.usage       = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize  = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

void RendererState::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0; // Optional
    copyRegion.dstOffset = 0; // Optional
    copyRegion.size      = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    endSingleTimeCommands(commandBuffer);
}

void RendererState::updateUniforms(const TransformUniform& tform) {
    memcpy(uniformBuffersMapped[currentFrame], &tform, sizeof(tform));
}

void RendererState::createImage(std::uint32_t width, std::uint32_t height, VkFormat format,
                                VkImageTiling tiling, VkImageUsageFlags usage,
                                VkMemoryPropertyFlags properties, VkImage& image,
                                VkDeviceMemory& imageMemory) {
    VkImageCreateInfo imageInfo{};
    imageInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType     = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width  = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth  = 1;
    imageInfo.mipLevels     = 1;
    imageInfo.arrayLayers   = 1;
    imageInfo.format        = format;
    imageInfo.tiling        = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage         = usage;
    imageInfo.samples       = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize  = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(device, image, imageMemory, 0);
}

VkCommandBuffer RendererState::beginSingleTimeCommands() {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool        = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void RendererState::endSingleTimeCommands(VkCommandBuffer commandBuffer) {
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &commandBuffer;

    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);

    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

void RendererState::transitionImageLayout(VkImage image, VkFormat, VkImageLayout oldLayout,
                                          VkImageLayout newLayout) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkImageMemoryBarrier barrier{};
    barrier.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout           = oldLayout;
    barrier.newLayout           = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    barrier.image                           = image;
    barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel   = 0;
    barrier.subresourceRange.levelCount     = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount     = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
        newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage      = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
             newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage      = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else { throw std::invalid_argument("Unsupported layout transition"); }

    vkCmdPipelineBarrier(
        commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    endSingleTimeCommands(commandBuffer);
}

void RendererState::copyBufferToImage(VkBuffer buffer, VkImage image, std::uint32_t width,
                                      std::uint32_t height) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkBufferImageCopy region{};
    region.bufferOffset      = 0;
    region.bufferRowLength   = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel       = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount     = 1;

    region.imageOffset = {0, 0, 0};
    region.imageExtent = {width, height, 1};

    vkCmdCopyBufferToImage(
        commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    endSingleTimeCommands(commandBuffer);
}

VkImageView RendererState::createImageView(VkImage image, VkFormat format) {
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image                           = image;
    viewInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format                          = format;
    viewInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel   = 0;
    viewInfo.subresourceRange.levelCount     = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount     = 1;

    VkImageView imageView;
    if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create texture image view");
    }

    return imageView;
}
