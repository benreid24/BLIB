#ifndef BLIB_RENDER_VULKAN_ATTACHMENTBUFFERSET_HPP
#define BLIB_RENDER_VULKAN_ATTACHMENTBUFFERSET_HPP

#include <BLIB/Render/Vulkan/AttachmentBuffer.hpp>
#include <BLIB/Render/Vulkan/AttachmentSet.hpp>
#include <BLIB/Render/Vulkan/TextureFormat.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>
#include <BLIB/Vulkan.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
/**
 * @brief Generic set of N attachment buffers that can be used for framebuffer backing
 *
 * @tparam BufferCount The number of buffers to create
 * @ingroup Renderer
 */
template<std::uint32_t BufferCount>
class AttachmentBufferSet {
public:
    /**
     * @brief Creates an empty render buffer with no resources
     */
    AttachmentBufferSet()
    : owner(nullptr) {}

    /**
     * @brief Frees resources if not already destroyed
     */
    ~AttachmentBufferSet() = default;

    /**
     * @brief Frees prior images, if any, and creates new attachment buffers
     *
     * @param vulkaState Renderer vulkan state
     * @param size The size of the color and depth attachments to create
     * @param bufferFormats The formats to create the attachments with
     * @param usages How the attachments will be used
     */
    void create(VulkanState& vulkaState, const VkExtent2D& size,
                const std::array<VkFormat, BufferCount>& bufferFormats,
                const std::array<VkImageUsageFlags, BufferCount>& usages);

    /**
     * @brief Access the attachment buffer at the given index
     *
     * @param i The index of the attachment buffer to get
     * @return The attachment buffer at the given index
     */
    AttachmentBuffer& getBuffer(std::uint32_t i);

    /**
     * @brief Destroys the images, views, sampler, and frees GPU memory
     */
    void destroy();

    /**
     * @brief Queues destruction for later
     */
    void deferDestroy();

    /**
     * @brief Returns the attachment set to use to render to this target
     */
    const AttachmentSet& attachmentSet() const;

    /**
     * @brief Returns the size of the images contained in the buffer
     */
    const VkExtent2D& bufferSize() const;

private:
    VulkanState* owner;
    std::array<VkImage, BufferCount> images;
    std::array<VkImageView, BufferCount> views;
    AttachmentSet attachments;
    std::array<AttachmentBuffer, BufferCount> buffers;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<std::uint32_t BufferCount>
void AttachmentBufferSet<BufferCount>::create(
    VulkanState& vulkaState, const VkExtent2D& size,
    const std::array<VkFormat, BufferCount>& bufferFormats,
    const std::array<VkImageUsageFlags, BufferCount>& usages) {
    owner = &vulkaState;
    attachments.setRenderExtent(size);

    std::array<VkImageAspectFlags, BufferCount> aspects;
    for (std::uint32_t i = 0; i < BufferCount; ++i) {
        buffers[i].create(vulkaState, bufferFormats[i], usages[i], size);
        images[i]  = buffers[i].image();
        views[i]   = buffers[i].view();
        aspects[i] = VulkanState::guessImageAspect(bufferFormats[i], usages[i]);
    }
    attachments.init(images, views, aspects);
}

template<std::uint32_t BufferCount>
AttachmentBuffer& AttachmentBufferSet<BufferCount>::getBuffer(std::uint32_t i) {
    return buffers[i];
}

template<std::uint32_t BufferCount>
void AttachmentBufferSet<BufferCount>::destroy() {
    if (owner != nullptr) {
        owner = nullptr;
        for (auto& b : buffers) { b.destroy(); }
    }
}

template<std::uint32_t BufferCount>
void AttachmentBufferSet<BufferCount>::deferDestroy() {
    if (owner != nullptr) {
        owner = nullptr;
        for (auto& b : buffers) { b.deferDestroy(); }
    }
}

template<std::uint32_t BufferCount>
const AttachmentSet& AttachmentBufferSet<BufferCount>::attachmentSet() const {
    return attachments;
}

template<std::uint32_t BufferCount>
const VkExtent2D& AttachmentBufferSet<BufferCount>::bufferSize() const {
    return attachments.renderExtent();
}

} // namespace vk
} // namespace rc
} // namespace bl

#endif
