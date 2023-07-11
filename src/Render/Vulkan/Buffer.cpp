#include <BLIB/Render/Vulkan/Buffer.hpp>

#include <BLIB/Render/Vulkan/VulkanState.hpp>

namespace bl
{
namespace gfx
{
namespace vk
{
Buffer::Buffer()
: vulkanState(nullptr)
, buffer(nullptr)
, alloc{}
, mapped(nullptr)
, allocFlags(0)
, size(0)
, memPool{}
, usage{} {}

Buffer::~Buffer() { destroy(); }

bool Buffer::create(VulkanState& vs, VkDeviceSize bsize, VkMemoryPropertyFlags memProps,
                    VkBufferUsageFlags use, VmaAllocationCreateFlags flags) {
    doDefer();
    vulkanState = &vs;
    memPool     = memProps;
    size        = bsize;
    usage       = use;
    allocFlags  = flags;

    return doCreate();
}

bool Buffer::doCreate() {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size        = size;
    bufferInfo.usage       = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo allocInfo{};
    allocInfo.requiredFlags = memPool;
    allocInfo.usage         = VMA_MEMORY_USAGE_AUTO;
    allocInfo.flags         = allocFlags;

    VmaAllocationInfo resultInfo{};
    const VkResult result = vmaCreateBuffer(
        vulkanState->vmaAllocator, &bufferInfo, &allocInfo, &buffer, &alloc, &resultInfo);
    if (result != VK_SUCCESS) {
        vulkanState = nullptr;
        size        = 0;
        return false;
    }
    mapped = resultInfo.pMappedData;
    return true;
}

bool Buffer::createWithFallback(VulkanState& vs, VkDeviceSize bsize, VkMemoryPropertyFlags memProps,
                                VkMemoryPropertyFlags fallbackPool, VkBufferUsageFlags use,
                                VmaAllocationCreateFlags flags) {
    if (!create(vs, bsize, memProps, use, flags)) {
        return create(vs, bsize, fallbackPool, use, flags);
    }
    return true;
}

void Buffer::destroy() {
    if (created()) {
        vmaDestroyBuffer(vulkanState->vmaAllocator, buffer, alloc);
        vulkanState = nullptr;
        size        = 0;
    }
}

void Buffer::deferDestruction() {
    doDefer();
    vulkanState = nullptr;
    size        = 0;
}

void Buffer::doDefer() {
    if (created()) {
        vulkanState->cleanupManager.add([vs = vulkanState, buffer = buffer, alloc = alloc]() {
            vmaDestroyBuffer(vs->vmaAllocator, buffer, alloc);
        });
    }
}

bool Buffer::ensureSize(VkDeviceSize newSize) {
    if (newSize > size) {
        doDefer();

        const VkDeviceSize oldSize = size;
        const VkBuffer oldBuffer   = buffer;
        size                       = newSize;
        if (!doCreate()) { throw std::runtime_error("Failed to resize buffer"); }

        VkCommandBuffer commandBuffer = vulkanState->beginSingleTimeCommands();

        // copy old buffer into new
        VkBufferCopy copyCmd{};
        copyCmd.dstOffset = 0;
        copyCmd.size      = oldSize;
        copyCmd.srcOffset = 0;
        vkCmdCopyBuffer(commandBuffer, oldBuffer, buffer, 1, &copyCmd);

        // insert pipeline barrier
        VkBufferMemoryBarrier bufBarrier{};
        bufBarrier.sType         = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
        bufBarrier.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
        bufBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        bufBarrier.buffer        = buffer;
        bufBarrier.offset        = 0;
        bufBarrier.size          = oldSize;
        vkCmdPipelineBarrier(commandBuffer,
                             VK_PIPELINE_STAGE_TRANSFER_BIT,
                             VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
                             VK_DEPENDENCY_BY_REGION_BIT,
                             0,
                             nullptr,
                             1,
                             &bufBarrier,
                             0,
                             nullptr);

        vulkanState->endSingleTimeCommands(commandBuffer);

        return true;
    }
    return false;
}

void* Buffer::mapMemory() {
    vmaMapMemory(vulkanState->vmaAllocator, alloc, &mapped);
    return mapped;
}

void Buffer::unMapMemory() { vmaUnmapMemory(vulkanState->vmaAllocator, alloc); }

} // namespace vk
} // namespace gfx
} // namespace bl
