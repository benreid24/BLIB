#ifndef BLIB_RENDER_TRANSFERS_GENERICBUFFERPRIV_HPP
#define BLIB_RENDER_TRANSFERS_GENERICBUFFERPRIV_HPP

#include <BLIB/Render/Transfers/TransferContext.hpp>
#include <BLIB/Render/Vulkan/PerFrame.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>
#include <glad/vulkan.h>
#include <vk_mem_alloc.h>

namespace bl
{
namespace render
{
namespace tfr
{
namespace priv
{
template<bool DoubleBuffer, bool StagingRequired>
struct GenericBufferStorage {
    void create(vk::VulkanState& vulkanState, std::uint32_t size, VkMemoryPropertyFlags memProps,
                VkBufferUsageFlags usage);
    void doWrite(VkCommandBuffer commandBuffer, tfr::TransferContext& ctx, const void* data,
                 std::uint32_t offset, std::uint32_t len);
    void destroy(vk::VulkanState& vulkanState);
    constexpr VkBuffer current() const;
};

template<>
struct GenericBufferStorage<true, true> {
    void create(vk::VulkanState& vulkanState, std::uint32_t size, VkMemoryPropertyFlags memProps,
                VkBufferUsageFlags usage) {
        vulkanState.createDoubleBuffer(
            size, usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT, memProps, buffers, gpuMemory);
        const VkDeviceSize offset = vulkanState.createDoubleBuffer(
            size,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffers,
            stagingMemory);
        unsigned int i = 0;
        void* memoryMap;
        vkCheck(vkMapMemory(vulkanState.device,
                            stagingMemory,
                            0,
                            offset * Config::MaxConcurrentFrames,
                            0,
                            &memoryMap));
        mappedStaging.init(vulkanState, [this, memoryMap, &i, offset, size](void*& dest) {
            dest = static_cast<char*>(memoryMap) + offset * i;
            ++i;
        });
    }

    void doWrite(VkCommandBuffer commandBuffer, tfr::TransferContext&, const void* data,
                 std::uint32_t offset, std::uint32_t len) {
        std::memcpy(static_cast<char*>(mappedStaging.current()) + offset, data, len);
        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = offset;
        copyRegion.dstOffset = offset;
        copyRegion.size      = len;
        vkCmdCopyBuffer(commandBuffer, stagingBuffers.current(), buffers.current(), 1, &copyRegion);
    }

    void destroy(vk::VulkanState& vulkanState) {
        vkUnmapMemory(vulkanState.device, stagingMemory);
        buffers.cleanup([&vulkanState, this](VkBuffer& buffer) {
            vkDestroyBuffer(vulkanState.device, buffer, nullptr);
        });
        stagingBuffers.cleanup([&vulkanState, this](VkBuffer& buffer) {
            vkDestroyBuffer(vulkanState.device, buffer, nullptr);
        });
        vkFreeMemory(vulkanState.device, gpuMemory, nullptr);
        vkFreeMemory(vulkanState.device, stagingMemory, nullptr);
    }

    constexpr VkBuffer current() const { return buffers.current(); }

    vk::PerFrame<VkBuffer> buffers;
    VkDeviceMemory gpuMemory;
    vk::PerFrame<VkBuffer> stagingBuffers;
    vk::PerFrame<void*> mappedStaging;
    VkDeviceMemory stagingMemory;
};

template<>
struct GenericBufferStorage<true, false> {
    void create(vk::VulkanState& vulkanState, std::uint32_t size, VkMemoryPropertyFlags memProps,
                VkBufferUsageFlags usage) {
        const VkDeviceSize offset =
            vulkanState.createDoubleBuffer(size, usage, memProps, buffers, memory);
        unsigned int i = 0;
        void* memoryMap;
        vkCheck(vkMapMemory(
            vulkanState.device, memory, 0, offset * Config::MaxConcurrentFrames, 0, &memoryMap));
        mappedBuffers.init(vulkanState, [this, memoryMap, &i, offset, size](void*& dest) {
            dest = static_cast<char*>(memoryMap) + offset * i;
            ++i;
        });
    }

    void doWrite(VkCommandBuffer, tfr::TransferContext&, const void* data, std::uint32_t offset,
                 std::uint32_t len) {
        std::memcpy(static_cast<char*>(mappedBuffers.current()) + offset, data, len);
    }

    void destroy(vk::VulkanState& vulkanState) {
        vkUnmapMemory(vulkanState.device, memory);
        buffers.cleanup([&vulkanState, this](VkBuffer& buffer) {
            vkDestroyBuffer(vulkanState.device, buffer, nullptr);
        });
        vkFreeMemory(vulkanState.device, memory, nullptr);
    }

    constexpr VkBuffer current() const { return buffers.current(); }

    vk::PerFrame<VkBuffer> buffers;
    VkDeviceMemory memory;
    vk::PerFrame<void*> mappedBuffers;
};

template<>
struct GenericBufferStorage<false, true> {
    void create(vk::VulkanState& vulkanState, std::uint32_t size, VkMemoryPropertyFlags memProps,
                VkBufferUsageFlags usage) {
        vulkanState.createBuffer(size,
                                 usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                 0,
                                 memProps,
                                 &buffer,
                                 &alloc,
                                 &allocInfo);
    }

    void doWrite(VkCommandBuffer commandBuffer, tfr::TransferContext& ctx, const void* data,
                 std::uint32_t offset, std::uint32_t len) {
        VkBuffer stagingBuffer;
        void* dest;
        ctx.createTemporaryStagingBuffer(len, stagingBuffer, &dest);
        std::memcpy(dest, data, len);

        VkBufferCopy copyCmd{};
        copyCmd.dstOffset = offset;
        copyCmd.size      = len;
        copyCmd.srcOffset = 0;
        vkCmdCopyBuffer(commandBuffer, stagingBuffer, buffer, 1, &copyCmd);
    }

    void destroy(vk::VulkanState& vulkanState) {
        vmaDestroyBuffer(vulkanState.vmaAllocator, buffer, alloc);
    }

    constexpr VkBuffer current() const { return buffer; }

    VkBuffer buffer;
    VmaAllocation alloc;
    VmaAllocationInfo allocInfo;
};

template<>
struct GenericBufferStorage<false, false> {
    void create(vk::VulkanState& vulkanState, std::uint32_t size, VkMemoryPropertyFlags memProps,
                VkBufferUsageFlags usage) {
        vulkanState.createBuffer(
            size, usage, VMA_ALLOCATION_CREATE_MAPPED_BIT, memProps, &buffer, &alloc, &allocInfo);
    }

    void doWrite(VkCommandBuffer, tfr::TransferContext&, const void* data, std::uint32_t offset,
                 std::uint32_t len) {
        std::memcpy(static_cast<char*>(allocInfo.pMappedData) + offset, data, len);
    }

    void destroy(vk::VulkanState& vulkanState) {
        // think vma handles unmapping?
        vmaDestroyBuffer(vulkanState.vmaAllocator, buffer, alloc);
    }

    constexpr VkBuffer current() const { return buffer; }

    VkBuffer buffer;
    VmaAllocation alloc;
    VmaAllocationInfo allocInfo;
};

} // namespace priv
} // namespace tfr
} // namespace render
} // namespace bl

#endif
