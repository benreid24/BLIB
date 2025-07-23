#include <BLIB/Render/Vulkan/VulkanState.hpp>

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

#include <BLIB/Engine/Configuration.hpp>
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
VkPhysicalDeviceProperties* globalDeviceProperties = nullptr;

#ifdef BLIB_DEBUG
const std::unordered_set<std::string> RequestedValidationLayers{"VK_LAYER_KHRONOS_validation"};
#endif

constexpr std::array<const char*, 2> RequiredDeviceExtensions{
    VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_EXT_DEPTH_CLIP_ENABLE_EXTENSION_NAME};
constexpr std::array<const char*, 1> OptionalDeviceExtensions{VK_KHR_MAINTENANCE_4_EXTENSION_NAME};

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

    for (const char* ext : RequiredDeviceExtensions) {
        if (std::find_if(availableExtensions.begin(),
                         availableExtensions.end(),
                         [ext](const VkExtensionProperties& c) {
                             return strcmp(c.extensionName, ext) == 0;
                         }) == availableExtensions.end()) {
            BL_LOG_WARN << name << " is missing required extension " << ext;
            return false;
        }
    }

    return true;
}

int scoreDeviceForOptionalExtensions(VkPhysicalDevice device, const char* name) {
    std::uint32_t extensionCount;
    vkCheck(vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr));

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkCheck(vkEnumerateDeviceExtensionProperties(
        device, nullptr, &extensionCount, availableExtensions.data()));

    int score = 0;
    for (const char* ext : OptionalDeviceExtensions) {
        if (std::find_if(availableExtensions.begin(),
                         availableExtensions.end(),
                         [ext](const VkExtensionProperties& c) {
                             return strcmp(c.extensionName, ext) == 0;
                         }) == availableExtensions.end()) {
            BL_LOG_WARN << name << " is missing optional extension " << ext;
        }
        else { score += 500; }
    }

    return score;
}

int scorePhysicalDevice(VkPhysicalDevice device, const VkPhysicalDeviceProperties& deviceProperties,
                        const VkPhysicalDeviceFeatures& deviceFeatures,
                        const QueueFamilyLocator& queueFamilies, VkSurfaceKHR surface) {
    // requirements
    if (!deviceFeatures.geometryShader) { return -1; }
    if (!deviceFeatures.samplerAnisotropy) { return -1; }
    if (!queueFamilies.complete()) { return -1; }
    if (!deviceHasRequiredExtensions(device, deviceProperties.deviceName)) { return -1; }
    SwapChainSupportDetails swapChainDetails(device, surface);
    if (swapChainDetails.formats.empty() || swapChainDetails.presentModes.empty()) { return -1; }

    // preferred optional features
    int score = 0;
    score += deviceProperties.limits.maxImageDimension2D;
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) { score += 50000; }
    score += scoreDeviceForOptionalExtensions(device, deviceProperties.deviceName);

    return score;
}
} // namespace

VulkanState::VulkanState(engine::EngineWindow& window)
: window(window)
, device(nullptr)
, surface(nullptr)
, swapchain(*this, window.getSfWindow())
, transferEngine(*this)
, descriptorPool(*this)
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
    createVmaAllocator();
    sharedCommandPool.create(*this);
    swapchain.create();
    transferEngine.init();
    descriptorPool.init();
    shaderCache.init(device);

    globalDeviceProperties = &physicalDeviceProperties;
}

void VulkanState::cleanup() {
    cleanupManager.flush();
    descriptorPool.cleanup();
    transferEngine.cleanup();
    shaderCache.cleanup();
    swapchain.destroy();
    sharedCommandPool.cleanup();
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

void VulkanState::beginFrame(AttachmentSet*& renderFrame, VkCommandBuffer& commandBuffer) {
    swapchain.beginFrame(renderFrame, commandBuffer);
    cleanupManager.onFrameStart();
}

void VulkanState::completeFrame() {
    std::unique_lock lock(cbSubmitMutex);
    swapchain.completeFrame();
    currentFrame = (currentFrame + 1) % cfg::Limits::MaxConcurrentFrames;
}

void VulkanState::createInstance() {
    // app info
    const std::string appName =
        engine::Configuration::getOrDefault<std::string>("blib.app.name", "BLIB Application");
    VkApplicationInfo appInfo{};
    appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName   = appName.c_str();
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName        = "BLIB";
    appInfo.engineVersion      = VK_MAKE_VERSION(3, 0, 0);
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
    if (surface) {
        cleanupManager.add([instance = instance, surface = surface]() {
            vkDestroySurfaceKHR(instance, surface, nullptr);
        });
    }
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

    vkGetPhysicalDeviceFeatures(physicalDevice, &physicalDeviceFeatures);
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

    std::uint32_t extensionCount = 0;
    vkCheck(
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr));
    availableExtensions.resize(extensionCount);
    vkCheck(vkEnumerateDeviceExtensionProperties(
        physicalDevice, nullptr, &extensionCount, availableExtensions.data()));
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
    deviceFeatures.geometryShader = VK_TRUE; // TODO - disable point shadows instead of requiring?
    deviceFeatures.samplerAnisotropy = VK_TRUE;
    deviceFeatures.wideLines =
        physicalDeviceProperties.limits.lineWidthRange[1] > 1.f ? VK_TRUE : VK_FALSE;
    deviceFeatures.sampleRateShading = physicalDeviceFeatures.sampleRateShading;

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
    createInfo.vulkanApiVersion = extensionIsAvailable(VK_KHR_MAINTENANCE_4_EXTENSION_NAME) ?
                                      VK_API_VERSION_1_3 :
                                      VK_API_VERSION_1_2;
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

VkFormat VulkanState::findDepthFormat() {
    return findSupportedFormat(
        {VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D32_SFLOAT},
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

VkFormat VulkanState::findShadowMapFormat() {
    return findSupportedFormat({VK_FORMAT_D32_SFLOAT,
                                VK_FORMAT_D32_SFLOAT_S8_UINT,
                                VK_FORMAT_D24_UNORM_S8_UINT,
                                VK_FORMAT_D16_UNORM},
                               VK_IMAGE_TILING_OPTIMAL,
                               VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT &
                                   VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT);
}

VkFormat VulkanState::findHighPrecisionFormat() {
    return findSupportedFormat(
        {VK_FORMAT_R32G32B32A32_SFLOAT, VK_FORMAT_R16G16B16A16_SFLOAT, VK_FORMAT_R8G8B8A8_UNORM},
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT);
}

void VulkanState::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                               VmaAllocationCreateFlags allocFlags,
                               VkMemoryPropertyFlags properties, VkBuffer* buffer,
                               VmaAllocation* vmaAlloc, VmaAllocationInfo* vmaAllocInfo) {
    std::unique_lock lock(bufferAllocMutex);

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size        = size;
    bufferInfo.usage       = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo allocInfo{};
    allocInfo.requiredFlags = properties;
    allocInfo.usage         = VMA_MEMORY_USAGE_AUTO;
    allocInfo.flags         = allocFlags;

    vkCheck(vmaCreateBuffer(vmaAllocator, &bufferInfo, &allocInfo, buffer, vmaAlloc, vmaAllocInfo));
}

void VulkanState::createImage(std::uint32_t width, std::uint32_t height, std::uint32_t layerCount,
                              VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
                              VkImageCreateFlags imageFlags, VkMemoryPropertyFlags properties,
                              VkImage* image, VmaAllocation* vmaAlloc,
                              VmaAllocationInfo* vmaAllocInfo, VmaAllocationCreateFlags flags) {
    std::unique_lock lock(imageAllocMutex);

    VkImageCreateInfo imageInfo{};
    imageInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType     = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width  = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth  = 1;
    imageInfo.mipLevels     = 1;
    imageInfo.arrayLayers   = layerCount;
    imageInfo.format        = format;
    imageInfo.tiling        = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage         = usage;
    imageInfo.samples       = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.flags         = imageFlags;

    VmaAllocationCreateInfo allocInfo{};
    allocInfo.requiredFlags = properties;
    allocInfo.usage         = VMA_MEMORY_USAGE_AUTO;
    allocInfo.flags         = flags;

    vkCheck(vmaCreateImage(vmaAllocator, &imageInfo, &allocInfo, image, vmaAlloc, vmaAllocInfo));
}

VkResult VulkanState::submitCommandBuffer(const VkSubmitInfo& submitInfo, VkFence fence) {
    std::unique_lock lock(cbSubmitMutex);

    const auto r = vkQueueSubmit(graphicsQueue, 1, &submitInfo, fence);
    vkCheck(r);
    return r;
}

void VulkanState::transitionImageLayout(VkImage image, VkImageLayout oldLayout,
                                        VkImageLayout newLayout, std::uint32_t layerCount,
                                        VkImageAspectFlags aspect, std::uint32_t baseMipLevel,
                                        std::uint32_t mipLevelCount) {
    auto commandBuffer = sharedCommandPool.createBuffer();
    transitionImageLayout(commandBuffer,
                          image,
                          oldLayout,
                          newLayout,
                          layerCount,
                          aspect,
                          baseMipLevel,
                          mipLevelCount);
    commandBuffer.submit();
}

void VulkanState::transitionImageLayout(VkCommandBuffer commandBuffer, VkImage image,
                                        VkImageLayout oldLayout, VkImageLayout newLayout,
                                        std::uint32_t layerCount, VkImageAspectFlags aspect,
                                        std::uint32_t baseMipLevel, std::uint32_t mipLevelCount) {
    VkImageMemoryBarrier barrier{};
    barrier.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout           = oldLayout;
    barrier.newLayout           = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    barrier.image                           = image;
    barrier.subresourceRange.aspectMask     = aspect;
    barrier.subresourceRange.baseMipLevel   = baseMipLevel;
    barrier.subresourceRange.levelCount     = mipLevelCount;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount     = layerCount;

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
    else if (newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;

        sourceStage      = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL &&
             oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage =
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    }
    else if (newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR &&
             oldLayout == VK_IMAGE_LAYOUT_UNDEFINED) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
        sourceStage           = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage      = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    }
    else { throw std::invalid_argument("Unsupported layout transition"); }

    vkCmdPipelineBarrier(
        commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
}

void VulkanState::copyBufferToImage(VkBuffer buffer, VkImage image, std::uint32_t width,
                                    std::uint32_t height) {
    auto commandBuffer = sharedCommandPool.createBuffer();

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

    commandBuffer.submit();
}

VkImageView VulkanState::createImageView(VkImage image, VkFormat format,
                                         VkImageAspectFlags aspectFlags, std::uint32_t layerCount,
                                         VkImageViewType viewType, std::uint32_t mipLevels,
                                         std::uint32_t baseMipLevel) {
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image                           = image;
    viewInfo.viewType                        = viewType;
    viewInfo.format                          = format;
    viewInfo.subresourceRange.aspectMask     = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel   = baseMipLevel;
    viewInfo.subresourceRange.levelCount     = mipLevels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount     = layerCount;

    VkImageView imageView;
    if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create image view");
    }

    return imageView;
}

VkDeviceSize VulkanState::computeAlignedSize(VkDeviceSize len, VkDeviceSize align) {
    return (len + align - 1) & ~(align - 1);
}

VkFormatProperties VulkanState::getFormatProperties(VkFormat format) {
    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);
    return props;
}

bool VulkanState::extensionIsAvailable(const char* ext) const {
    for (const auto& e : availableExtensions) {
        if (strcmp(ext, e.extensionName) == 0) { return true; }
    }
    return false;
}

const VkPhysicalDeviceProperties& VulkanState::getPhysicalDeviceProperties() {
    return *globalDeviceProperties;
}

VkImageAspectFlags VulkanState::guessImageAspect(VkFormat, VkImageUsageFlags usage) {
    if (usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) { return VK_IMAGE_ASPECT_DEPTH_BIT; }
    return VK_IMAGE_ASPECT_COLOR_BIT;
}

} // namespace vk
} // namespace rc
} // namespace bl
