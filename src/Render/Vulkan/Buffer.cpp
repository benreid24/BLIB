#include <BLIB/Render/Vulkan/Buffer.hpp>

#include <BLIB/Render/Renderer.hpp>
#include <BLIB/Render/Vulkan/VulkanLayer.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
Buffer::Buffer()
: renderer(nullptr)
, buffer(nullptr)
, alloc{}
, mapped(nullptr)
, allocFlags(0)
, size(0)
, memPool{}
, usage{} {}

Buffer::~Buffer() { deferDestruction(); }

bool Buffer::create(Renderer& r, VkDeviceSize bsize, VkMemoryPropertyFlags memProps,
                    VkBufferUsageFlags use, VmaAllocationCreateFlags flags) {
    doDefer();
    renderer   = &r;
    memPool    = memProps;
    size       = bsize;
    usage      = use;
    allocFlags = flags;

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
    const VkResult result =
        renderer->vulkanState().createBuffer(bufferInfo, allocInfo, buffer, alloc, &resultInfo);
    if (result != VK_SUCCESS) {
        renderer = nullptr;
        size     = 0;
        return false;
    }
    mapped = resultInfo.pMappedData;
    return true;
}

bool Buffer::createWithFallback(Renderer& r, VkDeviceSize bsize, VkMemoryPropertyFlags memProps,
                                VkMemoryPropertyFlags fallbackPool, VkBufferUsageFlags use,
                                VmaAllocationCreateFlags flags) {
    if (!create(r, bsize, memProps, use, flags)) {
        return create(r, bsize, fallbackPool, use, flags);
    }
    return true;
}

void Buffer::destroy() {
    if (created()) {
        vmaDestroyBuffer(renderer->vulkanState().getVmaAllocator(), buffer, alloc);
        renderer = nullptr;
        size     = 0;
    }
}

void Buffer::deferDestruction() {
    doDefer();
    renderer = nullptr;
    size     = 0;
}

void Buffer::doDefer() {
    if (created()) {
        renderer->getCleanupManager().add(
            [vs = &renderer->vulkanState(), buffer = buffer, alloc = alloc]() {
                vmaDestroyBuffer(vs->getVmaAllocator(), buffer, alloc);
            });
    }
}

bool Buffer::ensureSize(VkDeviceSize newSize, bool skipCopy) {
    if (newSize > size) {
        doDefer();

        const VkDeviceSize oldSize = size;
        const VkBuffer oldBuffer   = buffer;
        size                       = newSize;
        if (!doCreate()) { throw std::runtime_error("Failed to resize buffer"); }

        if (!skipCopy) {
            auto commandBuffer = renderer->getSharedCommandPool().createBuffer();

            // barrier to ensure writes to old buffer are complete
            VkBufferMemoryBarrier barrier{};
            barrier.sType         = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
            barrier.buffer        = oldBuffer;
            barrier.offset        = 0;
            barrier.size          = oldSize;
            barrier.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
            vkCmdPipelineBarrier(commandBuffer,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT,
                                 VK_DEPENDENCY_BY_REGION_BIT,
                                 0,
                                 nullptr,
                                 1,
                                 &barrier,
                                 0,
                                 nullptr);

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

            commandBuffer.submit();

            return true;
        }
    }
    return false;
}

void* Buffer::mapMemory() {
    vmaMapMemory(renderer->vulkanState().getVmaAllocator(), alloc, &mapped);
    return mapped;
}

void Buffer::unMapMemory() { vmaUnmapMemory(renderer->vulkanState().getVmaAllocator(), alloc); }

void Buffer::insertPipelineBarrierBeforeChange() {
    auto cb = renderer->getSharedCommandPool().createBuffer();

    VkBufferMemoryBarrier barrier{};
    barrier.sType         = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    barrier.buffer        = buffer;
    barrier.offset        = 0;
    barrier.size          = size;
    barrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    barrier.dstAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
    vkCmdPipelineBarrier(cb,
                         VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                         VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_DEPENDENCY_BY_REGION_BIT,
                         0,
                         nullptr,
                         1,
                         &barrier,
                         0,
                         nullptr);

    cb.submit();
}

void Buffer::recordBarrier(VkCommandBuffer commandBuffer, VkPipelineStageFlags srcStages,
                           VkAccessFlags srcAccess, VkPipelineStageFlags dstStages,
                           VkAccessFlags dstAccess) {
    VkBufferMemoryBarrier barrier{};
    barrier.sType         = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    barrier.srcAccessMask = srcAccess;
    barrier.dstAccessMask = dstAccess;
    barrier.buffer        = buffer;
    barrier.offset        = 0;
    barrier.size          = size;
    vkCmdPipelineBarrier(
        commandBuffer, srcStages, dstStages, 0, 0, nullptr, 1, &barrier, 0, nullptr);
}

} // namespace vk
} // namespace rc
} // namespace bl
