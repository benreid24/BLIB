#ifndef BLIB_RENDER_VULKAN_ATTACHMENTIMAGESET_HPP
#define BLIB_RENDER_VULKAN_ATTACHMENTIMAGESET_HPP

#include <BLIB/Render/Vulkan/AttachmentSet.hpp>
#include <BLIB/Render/Vulkan/Image.hpp>
#include <BLIB/Render/Vulkan/SemanticTextureFormat.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>
#include <BLIB/Vulkan.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
/**
 * @brief Set of attachment buffers that can be used for framebuffer backing
 *
 * @ingroup Renderer
 */
class AttachmentImageSet {
public:
    static constexpr std::uint32_t MaxBufferCount = AttachmentSet::MaxAttachments;

    /**
     * @brief Creates an empty render buffer with no resources
     */
    AttachmentImageSet();

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
                const vk::SemanticTextureFormat* bufferFormats, const VkImageUsageFlags* usages,
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

    /**
     * @brief Re-creates any attachments that have had their format changed
     *
     * @return True if any attachments were recreated, false otherwise
     */
    bool recreateForFormatChange();

private:
    VulkanState* owner;
    std::array<SemanticTextureFormat, MaxBufferCount> formats;
    std::array<VkImage, MaxBufferCount> images;
    std::array<VkImageView, MaxBufferCount> views;
    AttachmentSet attachments;
    std::array<Image, MaxBufferCount> buffers;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline Image& AttachmentImageSet::getBuffer(std::uint32_t i) { return buffers[i]; }

inline const AttachmentSet& AttachmentImageSet::attachmentSet() const { return attachments; }

inline const VkExtent2D& AttachmentImageSet::bufferSize() const {
    return attachments.getRenderExtent();
}

} // namespace vk
} // namespace rc
} // namespace bl

#endif
