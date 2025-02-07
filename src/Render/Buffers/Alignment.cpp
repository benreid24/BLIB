#include <BLIB/Render/Buffers/Alignment.hpp>

#include <BLIB/Render/Vulkan/VulkanState.hpp>
#include <glm/glm.hpp>

namespace bl
{
namespace rc
{
namespace buf
{
VkDeviceSize computeAlignment(VkDeviceSize dataSize, Alignment align) {
    switch (align) {
    case Alignment::Std140:
        return vk::VulkanState::computeAlignedSize(dataSize, sizeof(glm::vec4));
    case Alignment::Std430:
        return dataSize;
    case Alignment::UboBindOffset:
        return vk::VulkanState::computeAlignedSize(
            dataSize,
            vk::VulkanState::getPhysicalDeviceProperties().limits.minUniformBufferOffsetAlignment);

    case Alignment::SsboBindOffset:
        return vk::VulkanState::computeAlignedSize(
            dataSize,
            vk::VulkanState::getPhysicalDeviceProperties().limits.minStorageBufferOffsetAlignment);
    default:
        return dataSize;
    }
}

} // namespace buf
} // namespace rc
} // namespace bl
