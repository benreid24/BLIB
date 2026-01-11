#include <BLIB/Render/Buffers/Alignment.hpp>

#include <BLIB/Render/Vulkan/VulkanLayer.hpp>
#include <glm/glm.hpp>

namespace bl
{
namespace rc
{
namespace buf
{
VkDeviceSize computeAlignment(VkDeviceSize dataSize, Alignment align) {
    switch (align) {
    case Alignment::Packed:
        return dataSize;

    case Alignment::Std140:
        return vk::VulkanLayer::computeAlignedSize(dataSize, sizeof(glm::vec4));

    case Alignment::Std430:
        return dataSize;

    case Alignment::UboBindOffset:
        return vk::VulkanLayer::computeAlignedSize(
            dataSize,
            vk::VulkanLayer::getPhysicalDeviceProperties().limits.minUniformBufferOffsetAlignment);

    case Alignment::SsboBindOffset:
        return vk::VulkanLayer::computeAlignedSize(
            dataSize,
            vk::VulkanLayer::getPhysicalDeviceProperties().limits.minStorageBufferOffsetAlignment);
    default:
        return dataSize;
    }
}

} // namespace buf
} // namespace rc
} // namespace bl
