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

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline SetWriteHelper::SetWriteHelper() {}

inline void SetWriteHelper::hintWriteCount(std::size_t writeCount) {
    setWrites.reserve(setWrites.size() + writeCount);
}

inline void SetWriteHelper::hintBufferInfoCount(std::size_t bufferInfoCount) {
    const VkDescriptorBufferInfo* base = bufferInfos.data();
    bufferInfos.reserve(bufferInfos.size() + bufferInfoCount);
    if (base != bufferInfos.data()) {
        for (auto& write : setWrites) {
            write.pBufferInfo = bufferInfos.data() + (write.pBufferInfo - base);
        }
    }
}

inline void SetWriteHelper::hintImageInfoCount(std::size_t imageInfoCount) {
    const VkDescriptorImageInfo* base = imageInfos.data();
    imageInfos.reserve(imageInfos.size() + imageInfoCount);
    if (base != imageInfos.data()) {
        for (auto& write : setWrites) {
            write.pImageInfo = imageInfos.data() + (write.pImageInfo - base);
        }
    }
}

inline VkWriteDescriptorSet& SetWriteHelper::getNewSetWrite(VkDescriptorSet set) {
    VkWriteDescriptorSet& write = setWrites.emplace_back(VkWriteDescriptorSet{});
    write                       = {};
    write.sType                 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.descriptorCount       = 1;
    write.dstArrayElement       = 0;
    write.dstSet                = set;
    return write;
}

inline VkDescriptorBufferInfo& SetWriteHelper::getNewBufferInfo() {
    const VkDescriptorBufferInfo* base = bufferInfos.data();
    VkDescriptorBufferInfo& info       = bufferInfos.emplace_back(VkDescriptorBufferInfo{});
    if (base != bufferInfos.data()) {
        for (auto& write : setWrites) {
            if (write.pBufferInfo != nullptr) {
                write.pBufferInfo = bufferInfos.data() + (write.pBufferInfo - base);
            }
        }
    }
    return info;
}

inline VkDescriptorImageInfo& SetWriteHelper::getNewImageInfo() {
    const VkDescriptorImageInfo* base = imageInfos.data();
    VkDescriptorImageInfo& info       = imageInfos.emplace_back(VkDescriptorImageInfo{});
    if (base != imageInfos.data()) {
        for (auto& write : setWrites) {
            if (write.pImageInfo != nullptr) {
                write.pImageInfo = imageInfos.data() + (write.pImageInfo - base);
            }
        }
    }
    return info;
}

inline void SetWriteHelper::performWrite(VkDevice device) {
    if (!setWrites.empty()) {
        vkUpdateDescriptorSets(device, setWrites.size(), setWrites.data(), 0, nullptr);
        setWrites.clear();
        bufferInfos.clear();
    }
}

} // namespace ds
} // namespace rc
} // namespace bl

#endif
