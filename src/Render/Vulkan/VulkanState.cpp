#include <BLIB/Render/Vulkan/VulkanState.hpp>

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

#include <BLIB/Logging.hpp>
#include <BLIB/Render/Vulkan/Framebuffer.hpp>
#include <BLIB/Resources/FileSystem.hpp>
#include <Render/Vulkan/Utils/QueueFamilyLocator.hpp>
#include <Render/Vulkan/Utils/SwapChainSupportDetails.hpp>
#include <set>
#include <string>
#include <unordered_set>

namespace bl
{
namespace rc
{
namespace vk
{
namespace
{
#ifdef BLIB_DEBUG
const std::unordered_set<std::string> RequestedValidationLayers{"VK_LAYER_KHRONOS_validation",
                                                                "VK_LAYER_LUNARG_monitor"};
#endif

constexpr std::array<const char*, 1> RequiredDeviceExtensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};

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
    vkCheck(vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr));

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkCheck(vkEnumerateDeviceExtensionProperties(
        device, nullptr, &extensionCount, availableExtensions.data()));

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
    if (!deviceFeatures.samplerAnisotropy) return -1;
    if (!queueFamilies.complete()) return -1;
    if (!deviceHasRequiredExtensions(device, deviceProperties.deviceName)) return -1;
    SwapChainSupportDetails swapChainDetails(device, surface);
    if (swapChainDetails.formats.empty() || swapChainDetails.presentModes.empty()) return -1;

    // preferred optional features
    int score = 0;
    score += deviceProperties.limits.maxImageDimension2D;
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) { score += 50000; }

    return score;
}
} // namespace

VulkanState::VulkanState(engine::EngineWindow& window)
: window(window)
, device(nullptr)
, swapchain(*this, window.getSfWindow())
, transferEngine(*this)
, descriptorPool(*this)
, samplerCache(*this)
, currentFrame(0) {}

VulkanState::~VulkanState() {
    if (device != nullptr) { cleanup(); }
}

void VulkanState::init() {
    if (volkInitialize() != VK_SUCCESS) { throw std::runtime_error("Failed to get Vulkan loader"); }

    createInstance();
    volkLoadInstance(instance);
#ifdef BLIB_DEBUG
    setupDebugMessenger();
#endif
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    // volkLoadDevice(device);
    createVmaAllocator();
    createSharedCommandPool();
    swapchain.create(surface);
    transferEngine.init();
    descriptorPool.init();
    samplerCache.init();
    shaderCache.init(device);
}

void VulkanState::cleanup() {
    cleanupManager.flush();
    samplerCache.cleanup();
    descriptorPool.cleanup();
    transferEngine.cleanup();
    shaderCache.cleanup();
    swapchain.destroy();
    vkDestroyCommandPool(device, sharedCommandPool, nullptr);
    vmaDestroyAllocator(vmaAllocator);
    vkDestroyDevice(device, nullptr);
#ifdef BLIB_DEBUG
    cleanupDebugMessenger();
#endif
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
    device = nullptr;
}

void VulkanState::invalidateSwapChain() { swapchain.invalidate(); }

void VulkanState::beginFrame(StandardAttachmentSet*& renderFrame, VkCommandBuffer& commandBuffer) {
    swapchain.beginFrame(renderFrame, commandBuffer);
    cleanupManager.onFrameStart();
}

void VulkanState::completeFrame() {
    swapchain.completeFrame();
    currentFrame = (currentFrame + 1) % Config::MaxConcurrentFrames;
}

void VulkanState::createInstance() {
    // app info
    VkApplicationInfo appInfo{};
    appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName   = "Vulkan Sandbox"; // TODO - from config
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName        = "BLIB";
    appInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion         = VK_API_VERSION_1_3;

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
    vkCheck(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr));
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkCheck(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data()));
    std::unordered_set<std::string> extensionsSet;
    for (const VkExtensionProperties& ext : extensions) {
        extensionsSet.emplace(ext.extensionName);
    }

    // required extensions
    std::vector<const char*> requiredExtentions =
        engine::EngineWindow::getRequiredInstanceExtensions();
#ifdef BLIB_DEBUG
    requiredExtentions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    requiredExtentions.emplace_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif

    bool missing = false;
    for (const char* ext : requiredExtentions) {
        if (extensionsSet.find(ext) == extensionsSet.end()) {
            BL_LOG_ERROR << "Missing required extension: " << ext;
            missing = true;
        }
    }
    if (missing) { throw std::runtime_error("Missing required extension"); }

    createInfo.enabledExtensionCount   = requiredExtentions.size();
    createInfo.ppEnabledExtensionNames = requiredExtentions.data();

#ifdef BLIB_MACOS
    createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

    // actual instance creation
    const VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
    if (result != VK_SUCCESS) {
        BL_LOG_ERROR << "Got instance creation error code: " << result;
        throw std::runtime_error("Failed to create Vulkan instance");
    }
}

#ifdef BLIB_DEBUG
void VulkanState::setupDebugMessenger() {
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

void VulkanState::createSurface() {
    if (!window.createVulkanSurface(instance, surface)) {
        throw std::runtime_error("Failed to create Vulkan surface");
    }
}

void VulkanState::pickPhysicalDevice() {
    std::uint32_t deviceCount = 0;
    vkCheck(vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr));
    if (deviceCount == 0) { throw std::runtime_error("Failed to find GPUs with Vulkan support"); }
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkCheck(vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data()));

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

    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
}

void VulkanState::createLogicalDevice() {
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
#ifdef BLIB_DEBUG
    deviceFeatures.robustBufferAccess = VK_TRUE;
#endif

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

void VulkanState::createVmaAllocator() {
    VmaVulkanFunctions funcs{};
    funcs.vkGetPhysicalDeviceProperties           = vkGetPhysicalDeviceProperties;
    funcs.vkGetPhysicalDeviceMemoryProperties     = vkGetPhysicalDeviceMemoryProperties;
    funcs.vkAllocateMemory                        = vkAllocateMemory;
    funcs.vkFreeMemory                            = vkFreeMemory;
    funcs.vkMapMemory                             = vkMapMemory;
    funcs.vkUnmapMemory                           = vkUnmapMemory;
    funcs.vkFlushMappedMemoryRanges               = vkFlushMappedMemoryRanges;
    funcs.vkInvalidateMappedMemoryRanges          = vkInvalidateMappedMemoryRanges;
    funcs.vkBindBufferMemory                      = vkBindBufferMemory;
    funcs.vkBindImageMemory                       = vkBindImageMemory;
    funcs.vkGetBufferMemoryRequirements           = vkGetBufferMemoryRequirements;
    funcs.vkGetImageMemoryRequirements            = vkGetImageMemoryRequirements;
    funcs.vkCreateBuffer                          = vkCreateBuffer;
    funcs.vkDestroyBuffer                         = vkDestroyBuffer;
    funcs.vkCreateImage                           = vkCreateImage;
    funcs.vkDestroyImage                          = vkDestroyImage;
    funcs.vkCmdCopyBuffer                         = vkCmdCopyBuffer;
    funcs.vkGetBufferMemoryRequirements2KHR       = vkGetBufferMemoryRequirements2;
    funcs.vkGetImageMemoryRequirements2KHR        = vkGetImageMemoryRequirements2;
    funcs.vkBindBufferMemory2KHR                  = vkBindBufferMemory2;
    funcs.vkBindImageMemory2KHR                   = vkBindImageMemory2;
    funcs.vkGetPhysicalDeviceMemoryProperties2KHR = vkGetPhysicalDeviceMemoryProperties2;
    funcs.vkGetDeviceBufferMemoryRequirements     = vkGetDeviceBufferMemoryRequirements;
    funcs.vkGetDeviceImageMemoryRequirements      = vkGetDeviceImageMemoryRequirements;

    VmaAllocatorCreateInfo createInfo{};
    createInfo.instance         = instance;
    createInfo.device           = device;
    createInfo.physicalDevice   = physicalDevice;
    createInfo.vulkanApiVersion = VK_API_VERSION_1_3;
    createInfo.pVulkanFunctions = &funcs;

    vmaCreateAllocator(&createInfo, &vmaAllocator);
}

VkCommandPool VulkanState::createCommandPool(VkCommandPoolCreateFlags flags) {
    QueueFamilyLocator queueFamilyLocator;
    queueFamilyLocator.populate(physicalDevice, surface);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags            = flags;
    poolInfo.queueFamilyIndex = queueFamilyLocator.graphicsFamily.value();

    VkCommandPool commandPool;
    if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create command pool");
    }
    return commandPool;
}

void VulkanState::createSharedCommandPool() {
    sharedCommandPool = createCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
}

#ifdef BLIB_DEBUG
void VulkanState::cleanupDebugMessenger() {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) { func(instance, debugMessenger, nullptr); }
}
#endif

std::uint32_t VulkanState::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
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

VkFormat VulkanState::findSupportedFormat(const std::initializer_list<VkFormat>& candidates,
                                          VkImageTiling tiling, VkFormatFeatureFlags features) {
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR &&
            (props.linearTilingFeatures & features) == features) {
            return format;
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL &&
                 (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    throw std::runtime_error("failed to find supported format!");
}

void VulkanState::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                               VmaAllocationCreateFlags allocFlags,
                               VkMemoryPropertyFlags properties, VkBuffer* buffer,
                               VmaAllocation* vmaAlloc, VmaAllocationInfo* vmaAllocInfo) {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size        = size;
    bufferInfo.usage       = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo allocInfo{};
    allocInfo.requiredFlags = properties;
    allocInfo.usage         = VMA_MEMORY_USAGE_AUTO;
    allocInfo.flags         = allocFlags;

    vmaCreateBuffer(vmaAllocator, &bufferInfo, &allocInfo, buffer, vmaAlloc, vmaAllocInfo);
}

void VulkanState::createImage(std::uint32_t width, std::uint32_t height, VkFormat format,
                              VkImageTiling tiling, VkImageUsageFlags usage,
                              VkMemoryPropertyFlags properties, VkImage* image,
                              VmaAllocation* vmaAlloc, VmaAllocationInfo* vmaAllocInfo) {
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

    VmaAllocationCreateInfo allocInfo{};
    allocInfo.requiredFlags = properties;
    allocInfo.usage         = VMA_MEMORY_USAGE_AUTO;

    vkCheck(vmaCreateImage(vmaAllocator, &imageInfo, &allocInfo, image, vmaAlloc, vmaAllocInfo));
}

VkCommandBuffer VulkanState::beginSingleTimeCommands(VkCommandPool pool) {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool        = pool != nullptr ? pool : sharedCommandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkCheck(vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer));

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkCheck(vkBeginCommandBuffer(commandBuffer, &beginInfo));

    return commandBuffer;
}

void VulkanState::endSingleTimeCommands(VkCommandBuffer commandBuffer, VkCommandPool pool) {
    vkCheck(vkEndCommandBuffer(commandBuffer));

    VkSubmitInfo submitInfo{};
    submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &commandBuffer;

    vkCheck(vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE));
    vkCheck(vkQueueWaitIdle(graphicsQueue));

    vkFreeCommandBuffers(device, pool != nullptr ? pool : sharedCommandPool, 1, &commandBuffer);
}

void VulkanState::transitionImageLayout(VkImage image, VkFormat, VkImageLayout oldLayout,
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
    else if (newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage      = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage      = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
             newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;

        sourceStage      = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else { throw std::invalid_argument("Unsupported layout transition"); }

    vkCmdPipelineBarrier(
        commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    endSingleTimeCommands(commandBuffer);
}

void VulkanState::copyBufferToImage(VkBuffer buffer, VkImage image, std::uint32_t width,
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

VkImageView VulkanState::createImageView(VkImage image, VkFormat format,
                                         VkImageAspectFlags aspectFlags) {
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image                           = image;
    viewInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format                          = format;
    viewInfo.subresourceRange.aspectMask     = aspectFlags;
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

VkDeviceSize VulkanState::computeAlignedSize(VkDeviceSize len, VkDeviceSize align) {
    return (len + align - 1) & ~(align - 1);
}

} // namespace vk
} // namespace rc
} // namespace bl
