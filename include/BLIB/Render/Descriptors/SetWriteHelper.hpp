#ifndef BLIB_RENDER_DESCRIPTORS_SETWRITEHELPER
#define BLIB_RENDER_DESCRIPTORS_SETWRITEHELPER

#include <BLIB/Vulkan.hpp>
#include <vector>

namespace bl
{
namespace rc
{
namespace ds
{
/**
 * @brief Utility to perform arbitrary descriptor set writes using stack allocated memory
 *
 * @ingroup Renderer
 */
class SetWriteHelper {
public:
    /**
     * @brief Creates a noop writer
     */
    SetWriteHelper();

    /**
     * @brief Hints the writer to the number of set writes to allocate for
     *
     * @param writeCount The expected amount of set writes
     */
    void hintWriteCount(std::size_t writeCount);

    /**
     * @brief Hints the writer to the number of buffer infos to allocate for
     *
     * @param writeCount The expected amount of buffer infos
     */
    void hintBufferInfoCount(std::size_t bufferInfoCount);

    /**
     * @brief Hints the writer to the number of image infos to allocate for
     *
     * @param writeCount The expected amount of image infos
     */
    void hintImageInfoCount(std::size_t imageInfoCount);

    /**
     * @brief Creates and returns a new write operation to be applied
     *
     * @param set The set to write to
     * @return The write operation to be populated
     */
    VkWriteDescriptorSet& getNewSetWrite(VkDescriptorSet set);

    /**
     * @brief Creates and returns a buffer info to be used in a write operation
     *
     * @return The buffer info to be populated
     */
    VkDescriptorBufferInfo& getNewBufferInfo();

    /**
     * @brief Creates and returns an image info to be used in a write operation
     *
     * return The image info to be populated
     */
    VkDescriptorImageInfo& getNewImageInfo();

    /**
     * @brief Performs the descriptor write of the created operations
     *
     * @param device The Vulkan device to use
     */
    void performWrite(VkDevice device);

private:
    std::vector<VkWriteDescriptorSet> setWrites;
    std::vector<VkDescriptorBufferInfo> bufferInfos;
    std::vector<VkDescriptorImageInfo> imageInfos;
};

} // namespace ds
} // namespace rc
} // namespace bl

#endif
