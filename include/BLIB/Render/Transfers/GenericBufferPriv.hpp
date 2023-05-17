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
        buffers.emptyInit(vulkanState);
        allocs.emptyInit(vulkanState);
        for (unsigned int i = 0; i < Config::MaxConcurrentFrames; ++i) {
            vulkanState.createBuffer(size,
                                     usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                     0,
                                     memProps,
                                     &buffers.getRaw(i),
                                     &allocs.getRaw(i),
                                     nullptr);
        }
        stagingBuffers.init(vulkanState, [this, &vulkanState, size](StagingBuffer& buf) {
            VmaAllocationInfo allocInfo;
            vulkanState.createBuffer(size,
                                     VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                     VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                                         VMA_ALLOCATION_CREATE_MAPPED_BIT,
                                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                         VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                     &buf.buffer,
                                     &buf.alloc,
                                     &allocInfo);
            buf.mapped = allocInfo.pMappedData;
        });
    }

    void doWrite(VkCommandBuffer commandBuffer, tfr::TransferContext&, const void* data,
                 std::uint32_t offset, std::uint32_t len) {
        std::memcpy(static_cast<char*>(stagingBuffers.current().mapped) + offset, data, len);

        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = offset;
        copyRegion.dstOffset = offset;
        copyRegion.size      = len;
        vkCmdCopyBuffer(
            commandBuffer, stagingBuffers.current().buffer, buffers.current(), 1, &copyRegion);
    }

    void destroy(vk::VulkanState& vulkanState) {
        for (unsigned int i = 0; i < Config::MaxConcurrentFrames; ++i) {
            vmaDestroyBuffer(vulkanState.vmaAllocator, buffers.getRaw(i), allocs.getRaw(i));
        }
        stagingBuffers.cleanup([&vulkanState](StagingBuffer& buf) {
            vmaDestroyBuffer(vulkanState.vmaAllocator, buf.buffer, buf.alloc);
        });
    }

    constexpr VkBuffer current() const { return buffers.current(); }

    struct StagingBuffer {
        VmaAllocation alloc;
        VkBuffer buffer;
        void* mapped;
    };

    vk::PerFrame<VkBuffer> buffers;
    vk::PerFrame<VmaAllocation> allocs;
    vk::PerFrame<StagingBuffer> stagingBuffers;
};

template<>
struct GenericBufferStorage<true, false> {
    void create(vk::VulkanState& vulkanState, std::uint32_t size, VkMemoryPropertyFlags memProps,
                VkBufferUsageFlags usage) {
        buffers.emptyInit(vulkanState);
        allocs.emptyInit(vulkanState);
        mappedBuffers.emptyInit(vulkanState);

        for (unsigned int i = 0; i < Config::MaxConcurrentFrames; ++i) {
            VmaAllocationInfo allocInfo;
            vulkanState.createBuffer(size,
                                     usage,
                                     VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                                         VMA_ALLOCATION_CREATE_MAPPED_BIT,
                                     memProps,
                                     &buffers.getRaw(i),
                                     &allocs.getRaw(i),
                                     &allocInfo);
            mappedBuffers.getRaw(i) = allocInfo.pMappedData;
        }
    }

    void doWrite(VkCommandBuffer, tfr::TransferContext&, const void* data, std::uint32_t offset,
                 std::uint32_t len) {
        std::memcpy(static_cast<char*>(mappedBuffers.current()) + offset, data, len);
    }

    void destroy(vk::VulkanState& vulkanState) {
        for (unsigned int i = 0; i < Config::MaxConcurrentFrames; ++i) {
            vmaDestroyBuffer(vulkanState.vmaAllocator, buffers.getRaw(i), allocs.getRaw(i));
        }
    }

    constexpr VkBuffer current() const { return buffers.current(); }

    vk::PerFrame<VkBuffer> buffers;
    vk::PerFrame<VmaAllocation> allocs;
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
