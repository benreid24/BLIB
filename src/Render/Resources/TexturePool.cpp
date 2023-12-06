#include <BLIB/Render/Resources/TexturePool.hpp>

#include <BLIB/Render/Vulkan/StandardAttachmentBuffers.hpp>

namespace bl
{
namespace rc
{
namespace res
{
TexturePool::TexturePool(vk::VulkanState& vs)
: vulkanState(vs)
, textures(vs, MaxTextureCount, TextureArrayBindIndex)
, refCounts(MaxTextureCount)
, freeSlots(MaxTextureCount - BindlessTextureArray::MaxRenderTextures - 1)
, freeRtSlots(BindlessTextureArray::MaxRenderTextures)
, reverseFileMap(MaxTextureCount - BindlessTextureArray::MaxRenderTextures)
, reverseImageMap(MaxTextureCount - BindlessTextureArray::MaxRenderTextures) {
    toRelease.reserve(64);
}

void TexturePool::init() {
    // create descriptor layout
    VkDescriptorSetLayoutBinding setBindings[] = {textures.getLayoutBinding()};
    VkDescriptorSetLayoutCreateInfo descriptorCreateInfo{};
    descriptorCreateInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorCreateInfo.bindingCount = std::size(setBindings);
    descriptorCreateInfo.pBindings    = setBindings;
    if (VK_SUCCESS !=
        vkCreateDescriptorSetLayout(
            vulkanState.device, &descriptorCreateInfo, nullptr, &descriptorSetLayout)) {
        throw std::runtime_error("Failed to create texture pool descriptor set layout");
    }

    // create descriptor pool
    VkDescriptorPoolSize poolSize{};
    poolSize.type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSize.descriptorCount = MaxTextureCount * 2;

    VkDescriptorPoolCreateInfo poolCreate{};
    poolCreate.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolCreate.maxSets       = 2;
    poolCreate.poolSizeCount = 1;
    poolCreate.pPoolSizes    = &poolSize;
    if (VK_SUCCESS !=
        vkCreateDescriptorPool(vulkanState.device, &poolCreate, nullptr, &descriptorPool)) {
        throw std::runtime_error("Failed to create texture descriptor pool");
    }

    // allocate descriptor set
    VkDescriptorSet allocatedSets[2];
    VkDescriptorSetLayout setLayouts[2] = {descriptorSetLayout, descriptorSetLayout};
    VkDescriptorSetAllocateInfo setAlloc{};
    setAlloc.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    setAlloc.descriptorPool     = descriptorPool;
    setAlloc.descriptorSetCount = 2;
    setAlloc.pSetLayouts        = setLayouts;
    if (VK_SUCCESS != vkAllocateDescriptorSets(vulkanState.device, &setAlloc, allocatedSets)) {
        throw std::runtime_error("Failed to allocate texture descriptor set");
    }
    descriptorSet   = allocatedSets[0];
    rtDescriptorSet = allocatedSets[1];

    // create error texture pattern and init texture array
    constexpr unsigned int ErrorSize    = 1024;
    constexpr unsigned int ErrorBoxSize = 64;
    textures.getErrorPattern().create(ErrorSize, ErrorSize);
    for (unsigned int x = 0; x < ErrorSize; ++x) {
        for (unsigned int y = 0; y < ErrorSize; ++y) {
            const unsigned int xi = x / ErrorBoxSize;
            const unsigned int yi = y / ErrorBoxSize;
            if ((xi % 2) == (yi % 2)) {
                textures.getErrorPattern().setPixel(x, y, sf::Color(230, 66, 245));
            }
            else { textures.getErrorPattern().setPixel(x, y, sf::Color(255, 254, 196)); }
        }
    }
    textures.init(descriptorSet, rtDescriptorSet);
}

void TexturePool::cleanup() {
    TextureRef::disableCleanup();
    textures.cleanup();
    vkDestroyDescriptorPool(vulkanState.device, descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(vulkanState.device, descriptorSetLayout, nullptr);
}

void TexturePool::releaseUnused() {
    std::unique_lock lock(mutex);
    vkDeviceWaitIdle(vulkanState.device);
    releaseUnusedLocked();
}

void TexturePool::releaseTexture(const TextureRef& ref) {
    if (ref.id() == ErrorTextureId) return;

    std::unique_lock lock(mutex);

    if (refCounts[ref.id()] > 1) {
        BL_LOG_WARN << "Releasing texture " << ref.id() << " which still has "
                    << refCounts[ref.id()] << " references";
    }

    doRelease(ref.id());
}

void TexturePool::releaseUnusedLocked() {
    for (std::uint32_t i : toRelease) { doRelease(i); }
    toRelease.clear();
}

void TexturePool::doRelease(std::uint32_t i) {
    refCounts[i] = 0;

    if (i < reverseFileMap.size()) {
        freeSlots.release(i);
        if (reverseFileMap[i]) {
            fileMap.erase(*reverseFileMap[i]);
            reverseFileMap[i] = nullptr;
        }
        if (reverseImageMap[i]) {
            imageMap.erase(reverseImageMap[i]);
            reverseImageMap[i] = nullptr;
        }
    }
    else { freeRtSlots.release(i - reverseFileMap.size()); }

    std::array<BindlessTextureArray*, 1> arrays = {&textures};
    BindlessTextureArray::resetTexture(descriptorSet, rtDescriptorSet, arrays, i);
}

void TexturePool::bindDescriptors(VkCommandBuffer cb, VkPipelineLayout pipelineLayout,
                                  std::uint32_t setIndex, bool forRt) {
    const VkDescriptorSet ds = forRt ? rtDescriptorSet : descriptorSet;
    vkCmdBindDescriptorSets(
        cb, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, setIndex, 1, &ds, 0, nullptr);
}

void TexturePool::queueForRelease(std::uint32_t i) {
    std::unique_lock lock(mutex);
    toRelease.emplace_back(i);
}

TextureRef TexturePool::allocateTexture() {
    if (!freeSlots.available()) {
        releaseUnusedLocked();
        if (!freeSlots.available()) { throw std::runtime_error("All texture slots in use!"); }
    }

    const std::uint32_t i = freeSlots.allocate();
    refCounts[i].store(0);
    reverseFileMap[i]  = nullptr;
    reverseImageMap[i] = nullptr;

    return TextureRef{*this, textures.getTexture(i)};
}

void TexturePool::finalizeNewTexture(std::uint32_t i, VkSampler sampler) {
    std::array<BindlessTextureArray*, 1> arrays = {&textures};
    textures.getTexture(i).sampler              = sampler;
    textures.getTexture(i).createFromContentsAndQueue();
    BindlessTextureArray::commitTexture(descriptorSet, rtDescriptorSet, arrays, i);
}

TextureRef TexturePool::createTexture(const sf::Image& src, VkSampler sampler) {
    if (!sampler) { sampler = vulkanState.samplerCache.filteredEdgeClamped(); }

    std::unique_lock lock(mutex);

    TextureRef txtr      = allocateTexture();
    txtr.texture->altImg = &src;
    finalizeNewTexture(txtr.id(), sampler);

    return txtr;
}

TextureRef TexturePool::createTexture(const glm::u32vec2& size, VkSampler sampler) {
    if (!sampler) { sampler = vulkanState.samplerCache.filteredEdgeClamped(); }

    std::unique_lock lock(mutex);

    TextureRef txtr = allocateTexture();
    txtr->create(size, vk::Texture::DefaultFormat, 0);
    txtr->sampler = sampler;
    textures.updateTexture(txtr.get());

    return txtr;
}

TextureRef TexturePool::createRenderTexture(const glm::u32vec2& size, VkSampler sampler) {
    if (!sampler) { sampler = vulkanState.samplerCache.filteredEdgeClamped(); }

    std::unique_lock lock(mutex);

    // allocate id
    if (!freeRtSlots.available()) {
        releaseUnusedLocked();
        if (!freeRtSlots.available()) {
            throw std::runtime_error("All render texture slots in use!");
        }
    }

    const std::uint32_t i = freeRtSlots.allocate() + reverseFileMap.size();
    refCounts[i].store(0);

    // init texture
    TextureRef txtr{*this, textures.getTexture(i)};
    txtr->create(size,
                 vk::StandardAttachmentBuffers::DefaultColorFormat,
                 VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
    txtr->sampler = sampler;
    textures.updateTexture(txtr.get());

    return txtr;
}

TextureRef TexturePool::getOrLoadTexture(const std::string& path, VkSampler sampler) {
    if (!sampler) { sampler = vulkanState.samplerCache.filteredEdgeClamped(); }

    std::unique_lock lock(mutex);

    auto it = fileMap.find(path);
    if (it != fileMap.end()) {
        const auto rit = std::find(toRelease.begin(), toRelease.end(), it->second);
        if (rit != toRelease.end()) { toRelease.erase(rit); }
        return TextureRef{*this, textures.getTexture(it->second)};
    }

    TextureRef txtr = allocateTexture();
    textures.prepareTextureUpdate(txtr.id(), path);
    it                        = fileMap.try_emplace(path, txtr.id()).first;
    reverseFileMap[txtr.id()] = &it->first;
    finalizeNewTexture(txtr.id(), sampler);

    return txtr;
}

TextureRef TexturePool::getOrLoadTexture(const sf::Image& src, VkSampler sampler) {
    if (!sampler) { sampler = vulkanState.samplerCache.filteredEdgeClamped(); }

    std::unique_lock lock(mutex);

    auto it = imageMap.find(&src);
    if (it != imageMap.end()) {
        const auto rit = std::find(toRelease.begin(), toRelease.end(), it->second);
        if (rit != toRelease.end()) { toRelease.erase(rit); }
        return TextureRef{*this, textures.getTexture(it->second)};
    }

    TextureRef txtr = allocateTexture();
    textures.prepareTextureUpdate(txtr.id(), src);
    it                         = imageMap.try_emplace(&src, txtr.id()).first;
    reverseImageMap[txtr.id()] = &src;
    finalizeNewTexture(txtr.id(), sampler);

    return txtr;
}

void TexturePool::onFrameStart() { textures.commitDescriptorUpdates(); }

} // namespace res
} // namespace rc
} // namespace bl
