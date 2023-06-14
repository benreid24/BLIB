#include <BLIB/Render/Resources/TexturePool.hpp>

namespace bl
{
namespace render
{
namespace res
{
TexturePool::TexturePool(vk::VulkanState& vs)
: vulkanState(vs)
, textures(vs, MaxTextureCount, TextureArrayBindIndex)
, refCounts(MaxTextureCount)
, freeSlots(MaxTextureCount)
, reverseFileMap(MaxTextureCount) {
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
    textures.init(descriptorSet);
}

void TexturePool::cleanup() {
    textures.cleanup();
    vkDestroyDescriptorPool(vulkanState.device, descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(vulkanState.device, descriptorSetLayout, nullptr);
}

void TexturePool::releaseUnused() {
    std::unique_lock lock(mutex);
    vkDeviceWaitIdle(vulkanState.device);
    releaseUnusedLocked();
}

void TexturePool::releaseUnusedLocked() {
    std::array<BindlessTextureArray*, 1> arrays = {&textures};
    for (std::uint32_t i : toRelease) {
        refCounts[i] = 0;
        freeSlots.release(i);
        if (reverseFileMap[i]) {
            fileMap.erase(*reverseFileMap[i]);
            reverseFileMap[i] = nullptr;
        }
        BindlessTextureArray::resetTexture(descriptorSet, arrays, i);
    }
    toRelease.clear();
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
        if (!freeSlots.available()) { throw std::runtime_error("All texture slots in use!"); }
    }

    const std::uint32_t i = freeSlots.allocate();
    refCounts[i].store(0);
    reverseFileMap[i] = nullptr;

    return TextureRef{*this, textures.getTexture(i)};
}

void TexturePool::finalizeNewTexture(std::uint32_t i, VkSampler sampler) {
    std::array<BindlessTextureArray*, 1> arrays = {&textures};
    textures.getTexture(i).sampler              = sampler;
    BindlessTextureArray::commitTexture(descriptorSet, arrays, i);
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
    txtr->create(size);
    txtr->sampler = sampler;
    textures.updateTexture(txtr.get());

    return txtr;
}

TextureRef TexturePool::getOrLoadTexture(const std::string& path, VkSampler sampler) {
    if (!sampler) { sampler = vulkanState.samplerCache.filteredEdgeClamped(); }

    std::unique_lock lock(mutex);

    auto it = fileMap.find(path);
    if (it != fileMap.end()) { return TextureRef{*this, textures.getTexture(it->second)}; }

    TextureRef txtr = allocateTexture();
    textures.prepareTextureUpdate(txtr.id(), path);
    it                        = fileMap.try_emplace(path, txtr.id()).first;
    reverseFileMap[txtr.id()] = &it->first;
    finalizeNewTexture(txtr.id(), sampler);

    return txtr;
}

} // namespace res
} // namespace render
} // namespace bl
