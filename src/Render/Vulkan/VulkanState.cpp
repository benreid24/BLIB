#include <BLIB/Render/Vulkan/VulkanState.hpp>

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
namespace render
{
namespace
{
#ifdef BLIB_DEBUG
const std::unordered_set<std::string> RequestedValidationLayers{"VK_LAYER_KHRONOS_validation",
                                                                "VK_LAYER_LUNARG_monitor"};
#endif

const std::vector<const char*> RequiredDeviceExtensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};

constexpr std::uint32_t SPIRVMagicNumber         = 0x07230203;
constexpr std::uint32_t SPIRVMagicNumberReversed = 0x03022307;

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

VulkanState::VulkanState(sf::WindowBase& window)
: window(window)
, swapchain(*this, window)
, currentFrame(0) {
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
    createCommandPoolAndTransferBuffer();
    // TODO - create render pass
    swapchain.create(surface, renderPass);
}

VulkanState::~VulkanState() {
    swapchain.destroy();
    // TODO - destroy render pass
    vkFreeCommandBuffers(device, sharedCommandPool, 1, &transferCommandBuffer);
    vkDestroyCommandPool(device, sharedCommandPool, nullptr);
    vkDestroyDevice(device, nullptr);
#ifdef BLIB_DEBUG
    cleanupDebugMessenger();
#endif
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
}

void VulkanState::invalidateSwapChain() { swapchain.invalidate(); }

VkCommandBuffer VulkanState::beginFrame() { return swapchain.beginFrame(); }

void VulkanState::completeFrame() {
    swapchain.completeFrame();
    currentFrame = (currentFrame + 1) % Config::MaxConcurrentFrames;
}

void VulkanState::createInstance() {
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
        throw std::runtime_error("Failed to create Vulkcan surface");
    }
}

void VulkanState::pickPhysicalDevice() {
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

void VulkanState::createCommandPoolAndTransferBuffer() {
    sharedCommandPool = createCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    // create long-lived command-buffer for memory transfers
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool        = sharedCommandPool;
    allocInfo.commandBufferCount = 1;
    vkAllocateCommandBuffers(device, &allocInfo, &transferCommandBuffer);
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

void VulkanState::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
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

void VulkanState::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0; // Optional
    copyRegion.dstOffset = 0; // Optional
    copyRegion.size      = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    endSingleTimeCommands(commandBuffer);
}

void VulkanState::createImage(std::uint32_t width, std::uint32_t height, VkFormat format,
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

VkCommandBuffer VulkanState::beginSingleTimeCommands(VkCommandPool pool) {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool        = pool != nullptr ? pool : sharedCommandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void VulkanState::endSingleTimeCommands(VkCommandBuffer commandBuffer, VkCommandPool pool) {
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &commandBuffer;

    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);

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

VkImageView VulkanState::createImageView(VkImage image, VkFormat format) {
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

VkShaderModule VulkanState::createShaderModule(const std::string& path) {
    char* data;
    std::size_t len = 0;

    if (path[0] <= Config::BuiltInshaderIds::MaxId) {
        switch (path[0]) {
        case Config::BuiltInshaderIds::TestVertexShader:
            // TODO
            break;
        case Config::BuiltInshaderIds::TestFragmentShader:
            // TODO
            break;
        default:
            BL_LOG_ERROR << "Invalid built-in shader id: " << static_cast<int>(path[0]);
            throw std::runtime_error("Invalid built-in shader id");
        }
    }
    else {
        if (!resource::FileSystem::getData(path, &data, len) || len < 4) {
            BL_LOG_ERROR << "Failed to load shader: " << path;
            throw std::runtime_error("Failed to load shader");
        }
    }

    const std::uint32_t* u32data = reinterpret_cast<const std::uint32_t*>(data);
    if (u32data[0] != SPIRVMagicNumber && u32data[0] != SPIRVMagicNumberReversed) {
        // TODO - support shader compilation
        BL_LOG_ERROR << "Shader '" << path
                     << "' is not compiled. Shader compilation is not yet supported";
        throw std::runtime_error("Shader compilation is not yet supported");
    }

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = len;
    createInfo.pCode    = u32data;
    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }
    return shaderModule;
}

} // namespace render
} // namespace bl
