#ifndef BLIB_RENDER_PRIMITIVES_GENERICBUFFERPRIV_HPP
#define BLIB_RENDER_PRIMITIVES_GENERICBUFFERPRIV_HPP

#include <BLIB/Render/Transfers/TransferContext.hpp>
#include <BLIB/Render/Util/PerFrame.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>
#include <glad/vulkan.h>

namespace bl
{
namespace render
{
namespace prim
{
namespace priv
{
template<bool DoubleBuffer, bool StagingRequired>
struct GenericBufferStorage {
    void create(VulkanState& vulkanState, std::uint32_t size, VkMemoryPropertyFlags memProps,
                VkBufferUsageFlags usage);
    void doWrite(VkCommandBuffer commandBuffer, tfr::TransferContext& ctx, const void* data,
                 std::uint32_t offset, std::uint32_t len);
    void destroy(VulkanState& vulkanState);
    constexpr VkBuffer current() const;
};

template<>
struct GenericBufferStorage<true, true> {
    void create(VulkanState& vulkanState, std::uint32_t size, VkMemoryPropertyFlags memProps,
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
        // TODO - reduce to single memory map and use offset based on current frame
        mappedStaging.init(vulkanState, [this, &vulkanState, &i, offset, size](void*& dest) {
            vkMapMemory(vulkanState.device, stagingMemory, i * offset, size, 0, &dest);
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

    void destroy(VulkanState& vulkanState) {
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

    PerFrame<VkBuffer> buffers;
    VkDeviceMemory gpuMemory;
    PerFrame<VkBuffer> stagingBuffers;
    PerFrame<void*> mappedStaging;
    VkDeviceMemory stagingMemory;
};

template<>
struct GenericBufferStorage<true, false> {
    void create(VulkanState& vulkanState, std::uint32_t size, VkMemoryPropertyFlags memProps,
                VkBufferUsageFlags usage) {
        const VkDeviceSize offset =
            vulkanState.createDoubleBuffer(size, usage, memProps, buffers, memory);
        unsigned int i = 0;
        mappedBuffers.init(vulkanState, [this, &vulkanState, &i, offset, size](void*& dest) {
            vkMapMemory(vulkanState.device, memory, i * offset, size, 0, &dest);
            ++i;
        });
    }

    void doWrite(VkCommandBuffer commandBuffer, tfr::TransferContext&, const void* data,
                 std::uint32_t offset, std::uint32_t len) {
        std::memcpy(static_cast<char*>(mappedBuffers.current()) + offset, data, len);
    }

    void destroy(VulkanState& vulkanState) {
        vkUnmapMemory(vulkanState.device, memory);
        buffers.cleanup([&vulkanState, this](VkBuffer& buffer) {
            vkDestroyBuffer(vulkanState.device, buffer, nullptr);
        });
        vkFreeMemory(vulkanState.device, memory, nullptr);
    }

    constexpr VkBuffer current() const { return buffers.current(); }

    PerFrame<VkBuffer> buffers;
    VkDeviceMemory memory;
    PerFrame<void*> mappedBuffers;
};

template<>
struct GenericBufferStorage<false, true> {
    void create(VulkanState& vulkanState, std::uint32_t size, VkMemoryPropertyFlags memProps,
                VkBufferUsageFlags usage) {
        vulkanState.createBuffer(
            size, usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT, memProps, buffer, gpuMemory);
    }

    void doWrite(VkCommandBuffer commandBuffer, tfr::TransferContext& ctx, const void* data,
                 std::uint32_t offset, std::uint32_t len) {
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingMemory;
        ctx.createTemporaryStagingBuffer(len, stagingBuffer, stagingMemory);

        void* dest = nullptr;
        vkMapMemory(ctx.device(), stagingMemory, 0, len, 0, &dest);
        std::memcpy(dest, data, len);
        vkUnmapMemory(ctx.device(), stagingMemory);

        VkBufferCopy copyCmd{};
        copyCmd.dstOffset = offset;
        copyCmd.size      = len;
        copyCmd.srcOffset = 0;
        vkCmdCopyBuffer(commandBuffer, stagingBuffer, buffer, 1, &copyCmd);
    }

    void destroy(VulkanState& vulkanState) {
        vkDestroyBuffer(vulkanState.device, buffer, nullptr);
        vkFreeMemory(vulkanState.device, gpuMemory, nullptr);
    }

    constexpr VkBuffer current() const { return buffer; }

    VkBuffer buffer;
    VkDeviceMemory gpuMemory;
};

template<>
struct GenericBufferStorage<false, false> {
    void create(VulkanState& vulkanState, std::uint32_t size, VkMemoryPropertyFlags memProps,
                VkBufferUsageFlags usage) {
        vulkanState.createBuffer(size, usage, memProps, buffer, memory);
        vkMapMemory(vulkanState.device, memory, 0, size, 0, &mappedBuffer);
    }

    void doWrite(VkCommandBuffer commandBuffer, tfr::TransferContext& ctx, const void* data,
                 std::uint32_t offset, std::uint32_t len) {
        std::memcpy(static_cast<char*>(mappedBuffer) + offset, data, len);
    }

    void destroy(VulkanState& vulkanState) {
        vkDestroyBuffer(vulkanState.device, buffer, nullptr);
        vkFreeMemory(vulkanState.device, memory, nullptr);
    }

    constexpr VkBuffer current() const { return buffer; }

    VkBuffer buffer;
    VkDeviceMemory memory;
    void* mappedBuffer;
};

} // namespace priv
} // namespace prim
} // namespace render
} // namespace bl

#endif
