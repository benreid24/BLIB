#include <BLIB/Render/Descriptors/SetWriteHelper.hpp>

namespace bl
{
namespace rc
{
namespace ds
{

SetWriteHelper::SetWriteHelper() {}

void SetWriteHelper::hintWriteCount(std::size_t writeCount) {
    setWrites.reserve(setWrites.size() + writeCount);
}

void SetWriteHelper::hintBufferInfoCount(std::size_t bufferInfoCount) {
    const VkDescriptorBufferInfo* base = bufferInfos.data();
    bufferInfos.reserve(bufferInfos.size() + bufferInfoCount);
    if (base != bufferInfos.data()) {
        for (auto& write : setWrites) {
            write.pBufferInfo = bufferInfos.data() + (write.pBufferInfo - base);
        }
    }
}

void SetWriteHelper::hintImageInfoCount(std::size_t imageInfoCount) {
    const VkDescriptorImageInfo* base = imageInfos.data();
    imageInfos.reserve(imageInfos.size() + imageInfoCount);
    if (base != imageInfos.data()) {
        for (auto& write : setWrites) {
            write.pImageInfo = imageInfos.data() + (write.pImageInfo - base);
        }
    }
}

VkWriteDescriptorSet& SetWriteHelper::getNewSetWrite(VkDescriptorSet set) {
    VkWriteDescriptorSet& write = setWrites.emplace_back(VkWriteDescriptorSet{});
    write                       = {};
    write.sType                 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.descriptorCount       = 1;
    write.dstArrayElement       = 0;
    write.dstSet                = set;
    return write;
}

VkDescriptorBufferInfo& SetWriteHelper::getNewBufferInfo() {
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

VkDescriptorImageInfo& SetWriteHelper::getNewImageInfo() {
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

void SetWriteHelper::performWrite(VkDevice device) {
    if (!setWrites.empty()) {
        vkUpdateDescriptorSets(device, setWrites.size(), setWrites.data(), 0, nullptr);
        setWrites.clear();
        bufferInfos.clear();
        imageInfos.clear();
    }
}

} // namespace ds
} // namespace rc
} // namespace bl