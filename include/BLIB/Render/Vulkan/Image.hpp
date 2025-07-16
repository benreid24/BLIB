#ifndef BLIB_RENDER_VULKAN_IMAGE_HPP
#define BLIB_RENDER_VULKAN_IMAGE_HPP

#include <BLIB/Render/Vulkan/ImageOptions.hpp>
#include <BLIB/Vulkan.hpp>
#include <glm/glm.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
struct VulkanState;
class AttachmentSet;

/**
 * @brief Utility class to create and manage an image
 *
 * @ingroup Renderer
 */
class Image {
public:
    /**
     * @brief Creates an uninitialized image
     */
    Image();

    /**
     * @brief Destroys the image
     */
    ~Image();

    /**
     * @brief Creates or resizes the image. Noop if extent is unchanged
     *
     * @param vulkanState Renderer Vulkan state
     * @param options The options to use for the image creation
     */
    void create(VulkanState& vulkanState, const ImageOptions& options);

    /**
     * @brief Resizes the image to the new size, optionally copying over the old contents
     *
     * @param newSize The new image size
     * @param copyContents Whether to copy old contents to the new image or not
     */
    void resize(const glm::u32vec2& newSize, bool copyContents);

    /**
     * @brief Clears the color image and transitions it to
     *        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
     *
     * @param clearColor The color to clear to
     */
    void clearAndPrepareForSampling(VkClearColorValue clearColor = {{0.f, 0.f, 0.f, 1.f}});

    /**
     * @brief Clears the color image and transitions it to
     *        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
     *
     * @param commandBuffer The command buffer to issue commands into
     * @param clearColor The color to clear to
     */
    void clearAndPrepareForSampling(VkCommandBuffer commandBuffer,
                                    VkClearColorValue clearColor = {{0.f, 0.f, 0.f, 1.f}});

    /**
     * @brief Clears the depth image and transitions it to
     *        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
     *
     * @param clearColor The color to clear to
     */
    void clearDepthAndPrepareForSampling(VkClearDepthStencilValue clearColor = {1.f, 0});

    /**
     * @brief Clears the depth image and transitions it to
     *        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
     *
     * @param commandBuffer The command buffer to submit commands into
     * @param clearColor The color to clear to
     */
    void clearDepthAndPrepareForSampling(VkCommandBuffer commandBuffer,
                                         VkClearDepthStencilValue clearColor = {1.f, 0});

    /**
     * @brief Clears the depth image and transitions it to a final layout
     *
     * @param commandBuffer The command buffer to submit commands into
     * @param finalLayout The final layout to transition to
     * @param clearColor The color to clear to
     */
    void clearDepthAndTransition(
        VkCommandBuffer commandBuffer,
        VkImageLayout finalLayout           = VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL,
        VkClearDepthStencilValue clearColor = {1.f, 0});

    /**
     * @brief Destroys the image
     */
    void destroy();

    /**
     * @brief Queues destruction for later
     */
    void deferDestroy();

    /**
     * @brief The Vulkan handle to the image of the image
     */
    VkImage getImage() const;

    /**
     * @brief Vulkan handle to the view of the image
     */
    VkImageView getView() const;

    /**
     * @brief The Vulkan handle to the image of the image
     */
    const VkImage* getImagePointer() const;

    /**
     * @brief Vulkan handle to the view of the image
     */
    const VkImageView* getViewPointer() const;

    /**
     * @brief Returns the size of the image
     */
    const VkExtent2D& getSize() const;

    /**
     * @brief Returns the format of the image
     */
    VkFormat getFormat() const;

    /**
     * @brief Returns the aspect of the image
     */
    VkImageAspectFlags getAspect() const;

    /**
     * @brief Returns the usage of this image
     */
    VkImageUsageFlags getUsage() const;

    /**
     * @brief Returns the view type of the image
     */
    VkImageViewType getViewType() const;

    /**
     * @brief Returns the number of layers in the image
     */
    std::uint32_t getLayerCount() const;

    /**
     * @brief Returns the current layout of the image
     */
    VkImageLayout getCurrentLayout() const;

    /**
     * @brief Issues a pipeline barrier to a temporary command buffer to transition the image layout
     *
     * @param newLayout The new layout to transition to
     * @param useUndefinedPreviousLayout Use undefined image layout instead of the last known
     */
    void transitionLayout(VkImageLayout newLayout, bool useUndefinedPreviousLayout = false);

    /**
     * @brief Issues a pipeline barrier to a given command buffer to transition the image layout
     *
     * @param commandBuffer The command buffer to issue the barrier into
     * @param newLayout The new layout to transition to
     * @param useUndefinedPreviousLayout Use undefined image layout instead of the last known
     */
    void transitionLayout(VkCommandBuffer commandBuffer, VkImageLayout newLayout,
                          bool useUndefinedPreviousLayout = false);

    /**
     * @brief Notifies the image of its layout being changed externally
     *
     * @param newLayout The new layout the image is in
     */
    void notifyNewLayout(VkImageLayout newLayout);

    /**
     * @brief Returns the memory type the image lives in
     */
    VkMemoryPropertyFlags getMemoryLocation() const;

    /**
     * @brief Returns whether the image has been created or not
     */
    bool isCreated() const;

    /**
     * @brief Returns the number of samples in the image
     */
    VkSampleCountFlagBits getSampleCount() const;

private:
    VulkanState* vulkanState;
    VmaAllocation alloc;
    VkImage imageHandle;
    VkImageView viewHandle;
    ImageOptions createOptions;
    VkImageLayout currentLayout;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline VkImage Image::getImage() const { return imageHandle; }

inline VkImageView Image::getView() const { return viewHandle; }

inline const VkImage* Image::getImagePointer() const { return &imageHandle; }

inline const VkImageView* Image::getViewPointer() const { return &viewHandle; }

inline const VkExtent2D& Image::getSize() const { return createOptions.extent; }

inline VkFormat Image::getFormat() const { return createOptions.format; }

inline VkImageAspectFlags Image::getAspect() const { return createOptions.aspect; }

inline VkImageUsageFlags Image::getUsage() const { return createOptions.usage; }

inline VkImageLayout Image::getCurrentLayout() const { return currentLayout; }

inline VkMemoryPropertyFlags Image::getMemoryLocation() const {
    return createOptions.memoryLocation;
}

inline bool Image::isCreated() const { return vulkanState != nullptr; }

inline VkSampleCountFlagBits Image::getSampleCount() const { return createOptions.samples; }

} // namespace vk
} // namespace rc
} // namespace bl

#endif
