#ifndef BLIB_RENDER_TEXTUREEXPORT_HPP
#define BLIB_RENDER_TEXTUREEXPORT_HPP

#include <BLIB/Util/NonCopyable.hpp>
#include <BLIB/Vulkan.hpp>
#include <SFML/Graphics/Image.hpp>
#include <atomic>
#include <condition_variable>
#include <mutex>

namespace bl
{
namespace rc
{
namespace vk
{
struct VulkanLayer;
}

namespace tfr
{
class TextureExporter;

/**
 * @brief An instance of a texture export command. Use TextureExporter to get this
 *
 * @ingroup Transfers
 */
class TextureExport : private util::NonCopyable {
public:
    /**
     * @brief Creates a texture export job. Use TextureExporter, do not use this directly
     *
     * @param vs Renderer Vulkan state
     * @param owner The texture exporter instance that owns this export
     * @param src The image to copy
     * @param srcLayout The layout of the image to copy
     * @param srcExtent The extent of the image copy
     * @param srcAspect The aspect of the image to copy
     * @param srcFormat The format of the image to copy
     */
    TextureExport(vk::VulkanLayer& vs, TextureExporter& owner, VkImage src, VkImageLayout srcLayout,
                  VkExtent3D srcExtent, VkImageAspectFlags srcAspect, VkFormat srcFormat);

    /**
     * @brief Releases resources
     */
    ~TextureExport();

    /**
     * @brief Returns whether the exported image is ready to be copied
     */
    bool imageReady() const;

    /**
     * @brief Blocks until the image export is ready. Do not call from primary engine thread or
     *        execution will be deadlocked
     */
    void wait();

    /**
     * @brief Copies the exported image to the destination sf::Image. Waits if the image is not yet
     *        ready. If called from primary engine thread then imageReady() must be checked first
     *
     * @param dst The sf::Image to store the result in
     */
    void copyImage(sf::Image& dst);

    /**
     * @brief Call when finished with the exported texture
     */
    void release();

private:
    vk::VulkanLayer& vulkanState;
    TextureExporter& owner;
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;
    VkFence fence;
    VmaAllocation destAlloc;
    VmaAllocationInfo destAllocInfo;
    VkImage destImage;
    bool requiresManualConversion;
    void* mapped;

    VkImage srcImage;
    VkImageLayout srcLayout;
    VkExtent3D srcExtent;
    VkImageAspectFlags srcAspect;
    VkFormat srcFormat;

    std::mutex progressMutex;
    std::condition_variable cv;
    std::atomic_bool inProgress;

    void performCopy();
    bool checkComplete();

    friend class TextureExporter;
};

} // namespace tfr
} // namespace rc
} // namespace bl

#endif
