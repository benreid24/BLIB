#include <BLIB/Render/Resources/TexturePool.hpp>

namespace bl
{
namespace render
{
TexturePool::TexturePool(VulkanState& vs)
: vulkanState(vs)
, textures(MaxTextureCount)
, refCounts(MaxTextureCount)
, freeSlots(MaxTextureCount)
, reverseFileMap(MaxTextureCount) {
    toRelease.reserve(64);
}

void TexturePool::init() {
    // create descriptor layout
    VkDescriptorSetLayoutBinding setBindings[1] = {{}};

    VkDescriptorSetLayoutBinding& texturePoolBinding = setBindings[TextureArrayBindIndex];
    texturePoolBinding.descriptorCount               = MaxTextureCount; // double?
    texturePoolBinding.binding                       = TextureArrayBindIndex;
    texturePoolBinding.stageFlags                    = VK_SHADER_STAGE_FRAGMENT_BIT;
    texturePoolBinding.descriptorType                = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    texturePoolBinding.pImmutableSamplers            = 0;

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
    poolSize.descriptorCount = MaxTextureCount; // double?

    VkDescriptorPoolCreateInfo poolCreate{};
    poolCreate.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolCreate.maxSets       = 1; // double buffer?
    poolCreate.poolSizeCount = 1;
    poolCreate.pPoolSizes    = &poolSize;
    if (VK_SUCCESS !=
        vkCreateDescriptorPool(vulkanState.device, &poolCreate, nullptr, &descriptorPool)) {
        throw std::runtime_error("Failed to create texture descriptor pool");
    }

    // allocate descriptor set
    VkDescriptorSetAllocateInfo setAlloc{};
    setAlloc.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    setAlloc.descriptorPool     = descriptorPool;
    setAlloc.descriptorSetCount = 1;
    setAlloc.pSetLayouts        = &descriptorSetLayout;
    if (VK_SUCCESS != vkAllocateDescriptorSets(vulkanState.device, &setAlloc, &descriptorSet)) {
        throw std::runtime_error("Failed to allocate texture descriptor set");
    }

    // create error texture
    constexpr unsigned int ErrorSize    = 1024;
    constexpr unsigned int ErrorBoxSize = 32;
    errorPattern.create(ErrorSize, ErrorSize);
    for (unsigned int x = 0; x < ErrorSize; ++x) {
        for (unsigned int y = 0; y < ErrorSize; ++y) {
            const unsigned int xi = x / ErrorBoxSize;
            const unsigned int yi = y / ErrorBoxSize;
            if ((xi % 2) == (yi % 2)) { errorPattern.setPixel(x, y, sf::Color(230, 66, 245)); }
            else { errorPattern.setPixel(x, y, sf::Color(255, 254, 196)); }
        }
    }
    errorTexture.externalContents = &errorPattern;
    errorTexture.createFromContentsAndQueue(vulkanState);
    errorTexture.sampler = vulkanState.samplerCache.filteredRepeated();
    vulkanState.transferEngine.executeTransfers();

    // init all textures to the error texture and init the descriptor set
    for (Texture& txtr : textures) { txtr = errorTexture; }
    writeAllDescriptors();
}

void TexturePool::cleanup() {
    errorTexture.cleanup(vulkanState);
    for (unsigned int i = 0; i < MaxTextureCount; ++i) {
        if (freeSlots.isAllocated(i)) { textures[i].cleanup(vulkanState); }
    }
    vkDestroyDescriptorPool(vulkanState.device, descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(vulkanState.device, descriptorSetLayout, nullptr);
}

void TexturePool::writeAllDescriptors() {
    VkDescriptorImageInfo imageInfos[MaxTextureCount]; // ~100kb, should be fine on stack
    for (unsigned int i = 0; i < MaxTextureCount; ++i) {
        imageInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfos[i].imageView   = textures[i].view;
        imageInfos[i].sampler     = textures[i].sampler;
    }

    VkWriteDescriptorSet setWrite{};
    setWrite.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    setWrite.descriptorCount = MaxTextureCount;
    setWrite.dstBinding      = TextureArrayBindIndex;
    setWrite.dstArrayElement = 0;
    setWrite.dstSet          = descriptorSet;
    setWrite.pImageInfo      = imageInfos;
    setWrite.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    vkUpdateDescriptorSets(vulkanState.device, 1, &setWrite, 0, nullptr);
}

void TexturePool::writeDescriptor(std::uint32_t i) {
    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView   = textures[i].view;
    imageInfo.sampler     = textures[i].sampler;

    VkWriteDescriptorSet setWrite{};
    setWrite.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    setWrite.descriptorCount = 1;
    setWrite.dstBinding      = TextureArrayBindIndex;
    setWrite.dstArrayElement = i; // correct?
    setWrite.dstSet          = descriptorSet;
    setWrite.pImageInfo      = &imageInfo;
    setWrite.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    vkUpdateDescriptorSets(vulkanState.device, 1, &setWrite, 0, nullptr);
}

void TexturePool::releaseUnused() {
    // TODO - gpu sync
    std::unique_lock lock(mutex);
    releaseUnusedLocked();
}

void TexturePool::releaseUnusedLocked() {
    const bool incrementalUpdate = toRelease.size() >= MaxTextureCount / 6;
    for (std::uint32_t i : toRelease) {
        refCounts[i] = 0;
        freeSlots.release(i);
        if (reverseFileMap[i]) {
            fileMap.erase(*reverseFileMap[i]);
            reverseFileMap[i] = nullptr;
        }
        textures[i].cleanup(vulkanState);
        textures[i] = errorTexture;
        if (incrementalUpdate) { writeDescriptor(i); }
    }
    toRelease.clear();

    if (!incrementalUpdate) { writeAllDescriptors(); }
}

void TexturePool::bindDescriptors(VkCommandBuffer cb, VkPipelineLayout pipelineLayout,
                                  std::uint32_t setIndex) {
    vkCmdBindDescriptorSets(cb,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            pipelineLayout,
                            setIndex,
                            1,
                            &descriptorSet,
                            0,
                            nullptr);
}

void TexturePool::queueForRelease(std::uint32_t i) {
    std::unique_lock lock(mutex);
    toRelease.emplace_back(i);
}

TextureRef TexturePool::allocateTexture() {
    if (!freeSlots.available()) {
        releaseUnusedLocked();
        if (!freeSlots.available()) { throw std::runtime_error("All texture slots in used!"); }
    }

    const std::uint32_t i = freeSlots.allocate();
    refCounts[i].store(0);
    reverseFileMap[i] = nullptr;

    return TextureRef{*this, textures[i]};
}

void TexturePool::finalizeNewTexture(std::uint32_t i, VkSampler sampler) {
    textures[i].sampler = sampler;
    textures[i].createFromContentsAndQueue(vulkanState);
    writeDescriptor(i);
}

TextureRef TexturePool::createTexture(glm::u32vec2 size, VkSampler sampler) {
    if (!sampler) { sampler = vulkanState.samplerCache.filteredEdgeClamped(); }

    std::unique_lock lock(mutex);

    // TODO - create empty texture of size
    return allocateTexture();
}

TextureRef TexturePool::createTexture(const sf::Image& src, VkSampler sampler) {
    if (!sampler) { sampler = vulkanState.samplerCache.filteredEdgeClamped(); }

    std::unique_lock lock(mutex);

    TextureRef txtr        = allocateTexture();
    txtr.texture->contents = src;
    finalizeNewTexture(txtr.id(), sampler);

    return txtr;
}

TextureRef TexturePool::getOrLoadTexture(const std::string& path, VkSampler sampler) {
    if (!sampler) { sampler = vulkanState.samplerCache.filteredEdgeClamped(); }

    std::unique_lock lock(mutex);

    auto it = fileMap.find(path);
    if (it != fileMap.end()) { return TextureRef{*this, textures[it->second]}; }

    TextureRef txtr = allocateTexture();
    if (!txtr.texture->contents.loadFromFile(path)) {
        txtr.texture->externalContents = &errorPattern;
    }
    it                        = fileMap.try_emplace(path, txtr.id()).first;
    reverseFileMap[txtr.id()] = &it->first;
    finalizeNewTexture(txtr.id(), sampler);

    return txtr;
}

} // namespace render
} // namespace bl
