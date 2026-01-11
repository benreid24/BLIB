#ifndef BLIB_RENDER_VULKAN_IMAGE_HPP
#define BLIB_RENDER_VULKAN_IMAGE_HPP

#include <BLIB/Render/Vulkan/ImageOptions.hpp>
#include <BLIB/Vulkan.hpp>
#include <SFML/Graphics/Image.hpp>
#include <glm/glm.hpp>

namespace bl
{
namespace rc
{
class Renderer;

namespace vk
{
/**
 * @brief Utility class to create and manage an image
 *
 * @ingroup Renderer
 */
class Image {
public:
    /**
     * @brief Computes the number of mip levels that should be generated for an image
     *
     * @param width The width of the image in pixels
     * @param height The height of the image in pixels
     * @return The number of mip levels to generate
     */
    static std::uint32_t computeMipLevelsForGeneration(std::uint32_t width, std::uint32_t height);

    /**
     * @brief Determines the number of mip levels present in an existing image. Must have been
     *        generated using this algorithm otherwise the result may be incorrect
     *
     * @param width The width of the pre-generated mip image in pixels
     * @param height The height of the pre-generated mip image in pixels
     * @return The number of mip levels present in the image
     */
    static std::uint32_t determineMipLevelsFromExisting(std::uint32_t width, std::uint32_t height);

    /**
     * @brief Creates a new image with mipmaps generated from the given image
     *
     * @param image The image to generate mipmaps for
     * @return An image with mipmaps generated from the given image
     */
    static sf::Image genMipMapsOnCpu(const sf::Image& image);

    /**
     * @brief Computes and returns the bounding box of the requested mip level
     *
     * @param size The size of the first mip level
     * @param mipLevel The index of the mip level to fetch
     * @return The bounding box of the requested mip level
     */
    static sf::IntRect getMipLevelBounds(const glm::u32vec2& size, std::uint32_t mipLevel);

    /**
     * @brief Iterative method to compute the bounds of the next mip level down from the current
     *
     * @param currentLevel The bounds of the current mip level
     * @param currentIndex The index of the current mip level
     * @return The bounds of the next mip level
     */
    static sf::IntRect getNextMipLevelBounds(const sf::IntRect& currentLevel,
                                             std::uint32_t currentIndex);

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
     * @param vulkanState Renderer instance
     * @param options The options to use for the image creation
     */
    void create(Renderer& vulkanState, const ImageOptions& options);

    /**
     * @brief Resizes the image to the new size, optionally copying over the old contents
     *
     * @param newSize The new image size
     * @param copyContents Whether to copy old contents to the new image or not
     */
    void resize(const glm::u32vec2& newSize, bool copyContents);

    /**
     * @brief Clears the depth image and transitions it to a final layout
     *
     * @param finalLayout The final layout to transition to
     * @param clearColor The color to clear to
     */
    void clearAndTransition(VkImageLayout finalLayout, VkClearValue clearColor);

    /**
     * @brief Clears the depth image and transitions it to a final layout
     *
     * @param commandBuffer The command buffer to submit commands into
     * @param finalLayout The final layout to transition to
     * @param clearColor The color to clear to
     */
    void clearAndTransition(VkCommandBuffer commandBuffer, VkImageLayout finalLayout,
                            VkClearValue clearColor);

    /**
     * @brief Returns whether the selected device can generate mipmaps on the GPU
     */
    bool canGenMipMapsOnGpu() const;

    /**
     * @brief Generates mipmaps for this image on the GPU
     *
     * @param finalLayout The final layout to transition to after mipmap generation
     */
    void genMipMapsOnGpu(VkImageLayout finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    /**
     * @brief Generates mipmaps for this image on the GPU
     *
     * @param commandBuffer The command buffer to issue commands into
     * @param finalLayout The final layout to transition to after mipmap generation
     */
    void genMipMapsOnGpu(VkCommandBuffer commandBuffer,
                         VkImageLayout finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

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
     * @brief Returns the number of mip levels in the image
     */
    std::uint32_t getLevelCount() const;

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

    /**
     * @brief Records a pipeline barrier into the specified Vulkan command buffer
     *
     * @param commandBuffer The Vulkan command buffer where the barrier will be recorded
     * @param srcStages The source pipeline stage mask indicating where the barrier starts
     * @param srcAccess The types of memory access before the barrier
     * @param dstStages The destination pipeline stage mask indicating where the barrier ends
     * @param dstAccess The types of memory access after the barrier
     * @param newLayout Optional new layout to transition to. Undefined to keep current layout
     */
    void recordBarrier(VkCommandBuffer commandBuffer, VkPipelineStageFlags srcStages,
                       VkAccessFlags srcAccess, VkPipelineStageFlags dstStages,
                       VkAccessFlags dstAccess,
                       VkImageLayout newLayout = VK_IMAGE_LAYOUT_UNDEFINED);

private:
    Renderer* renderer;
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

inline bool Image::isCreated() const { return renderer != nullptr; }

inline VkSampleCountFlagBits Image::getSampleCount() const { return createOptions.samples; }

inline std::uint32_t Image::getLevelCount() const { return createOptions.mipLevels; }

} // namespace vk
} // namespace rc
} // namespace bl

#endif
