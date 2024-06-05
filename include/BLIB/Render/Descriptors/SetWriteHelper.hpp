#ifndef BLIB_RENDER_DESCRIPTORS_SETWRITEHELPER
#define BLIB_RENDER_DESCRIPTORS_SETWRITEHELPER

#include <BLIB/Vulkan.hpp>

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
     *
     * @param set The set to write to
     */
    SetWriteHelper(VkDescriptorSet set);

    /**
     * @brief Creates and returns a new write operation to be applied
     *
     * @return The write operation to be populated
     */
    VkWriteDescriptorSet& getNewSetWrite();

    /**
     * @brief Creates and returns a buffer info to be used in a write operation
     *
     * @return The buffer info to be populated
     */
    VkDescriptorBufferInfo& getNewBufferInfo();

    /**
     * @brief Performs the descriptor write of the created operations
     *
     * @param device The Vulkan device to use
     */
    void performWrite(VkDevice device);

private:
    const VkDescriptorSet set;

    VkWriteDescriptorSet setWrites[8];
    unsigned int setWriteCount;

    VkDescriptorBufferInfo bufferInfos[16];
    unsigned int nextBufferInfo;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline SetWriteHelper::SetWriteHelper(VkDescriptorSet set)
: set(set)
, setWriteCount(0)
, nextBufferInfo(0) {}

inline VkWriteDescriptorSet& SetWriteHelper::getNewSetWrite() {
#ifdef BLIB_DEBUG
    if (setWriteCount >= std::size(setWrites)) { throw std::runtime_error("Too many set writes"); }
#endif

    VkWriteDescriptorSet& write = setWrites[setWriteCount];
    ++setWriteCount;
    write                 = {};
    write.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.descriptorCount = 1;
    write.dstArrayElement = 0;
    write.dstSet          = set;
    return write;
}

inline VkDescriptorBufferInfo& SetWriteHelper::getNewBufferInfo() {
#ifdef BLIB_DEBUG
    if (nextBufferInfo >= std::size(bufferInfos)) {
        throw std::runtime_error("Too many set writes");
    }
#endif

    VkDescriptorBufferInfo& info = bufferInfos[nextBufferInfo];
    ++nextBufferInfo;
    info = {};
    return info;
}

inline void SetWriteHelper::performWrite(VkDevice device) {
    vkUpdateDescriptorSets(device, setWriteCount, setWrites, 0, nullptr);
}

} // namespace ds
} // namespace rc
} // namespace bl

#endif
