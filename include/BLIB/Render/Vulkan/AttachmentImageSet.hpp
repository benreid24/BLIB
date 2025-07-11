#ifndef BLIB_RENDER_VULKAN_IMAGESET_HPP
#define BLIB_RENDER_VULKAN_IMAGESET_HPP

#include <BLIB/Render/Vulkan/AttachmentSet.hpp>
#include <BLIB/Render/Vulkan/Image.hpp>
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
 * @tparam MaxBufferCount The number of buffers to create
 * @ingroup Renderer
 */
template<std::uint32_t MaxBufferCount>
class AttachmentImageSet {
public:
    /**
     * @brief Creates an empty render buffer with no resources
     */
    AttachmentImageSet()
    : owner(nullptr) {}

    /**
     * @brief Frees resources if not already destroyed
     */
    ~AttachmentImageSet() = default;

    /**
     * @brief Frees prior images, if any, and creates new attachment buffers
     *
     * @param vulkaState Renderer vulkan state
     * @param count The number of buffers to create
     * @param size The size of the color and depth attachments to create
     * @param bufferFormats The formats to create the attachments with
     * @param usages How the attachments will be used
     * @param sampleCount The sample count to use for the non-resolve attachments
     * @param firstResolveAttachment The index of the first resolve attachment in the set
     */
    void create(VulkanState& vulkaState, unsigned int count, const VkExtent2D& size,
                const std::array<VkFormat, MaxBufferCount>& bufferFormats,
                const std::array<VkImageUsageFlags, MaxBufferCount>& usages,
                VkSampleCountFlagBits sampleCount    = VK_SAMPLE_COUNT_1_BIT,
                std::uint32_t firstResolveAttachment = 0);

    /**
     * @brief Access the attachment buffer at the given index
     *
     * @param i The index of the attachment buffer to get
     * @return The attachment buffer at the given index
     */
    Image& getBuffer(std::uint32_t i);

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
    std::array<VkImage, MaxBufferCount> images;
    std::array<VkImageView, MaxBufferCount> views;
    AttachmentSet attachments;
    std::array<Image, MaxBufferCount> buffers;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<std::uint32_t BufferCount>
void AttachmentImageSet<BufferCount>::create(
    VulkanState& vulkanState, unsigned int count, const VkExtent2D& size,
    const std::array<VkFormat, BufferCount>& bufferFormats,
    const std::array<VkImageUsageFlags, BufferCount>& usages, VkSampleCountFlagBits samples,
    std::uint32_t oi) {
    owner = &vulkanState;
    attachments.setRenderExtent(size);

    std::array<VkImageAspectFlags, BufferCount> aspects;
    for (std::uint32_t i = 0; i < BufferCount; ++i) {
        if (i < count) {
            const VkSampleCountFlagBits sc = i < oi ? samples : VK_SAMPLE_COUNT_1_BIT;
            aspects[i] = VulkanState::guessImageAspect(bufferFormats[i], usages[i]);
            buffers[i].create(vulkanState,
                              Image::Type::Image2D,
                              bufferFormats[i],
                              usages[i],
                              size,
                              aspects[i],
                              VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
                              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                              0,
                              VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM,
                              sc);
            images[i] = buffers[i].getImage();
            views[i]  = buffers[i].getView();
        }
        else {
            images[i] = nullptr;
            views[i]  = nullptr;
            if (buffers[i].isCreated()) { buffers[i].deferDestroy(); }
        }
    }
    attachments.init(images, views, aspects, buffers[0].getLayerCount(), count);
    attachments.setOutputIndex(oi);
}

template<std::uint32_t BufferCount>
Image& AttachmentImageSet<BufferCount>::getBuffer(std::uint32_t i) {
    return buffers[i];
}

template<std::uint32_t BufferCount>
void AttachmentImageSet<BufferCount>::destroy() {
    if (owner != nullptr) {
        owner = nullptr;
        for (auto& b : buffers) { b.destroy(); }
    }
}

template<std::uint32_t BufferCount>
void AttachmentImageSet<BufferCount>::deferDestroy() {
    if (owner != nullptr) {
        owner = nullptr;
        for (auto& b : buffers) { b.deferDestroy(); }
    }
}

template<std::uint32_t BufferCount>
const AttachmentSet& AttachmentImageSet<BufferCount>::attachmentSet() const {
    return attachments;
}

template<std::uint32_t BufferCount>
const VkExtent2D& AttachmentImageSet<BufferCount>::bufferSize() const {
    return attachments.renderExtent();
}

} // namespace vk
} // namespace rc
} // namespace bl

#endif
