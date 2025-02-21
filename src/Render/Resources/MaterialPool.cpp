#include <BLIB/Render/Resources/MaterialPool.hpp>

#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace res
{
namespace
{
bool defaultsInitialized = false;
sf::Image normalImage;
} // namespace

MaterialPool::MaterialPool(Renderer& renderer)
: renderer(renderer)
, freeIds(MaxMaterialCount) {}

MaterialRef MaterialPool::getOrCreateFromTexture(const res::TextureRef& texture) {
    std::unique_lock lock(mutex);

    checkLazyInit();

    if (texture.id() < textureIdToMaterialId.size() &&
        textureIdToMaterialId[texture.id()] != InvalidId) {
        return MaterialRef(this, textureIdToMaterialId[texture.id()]);
    }

    if (textureIdToMaterialId.size() <= texture.id()) {
        textureIdToMaterialId.resize(texture.id() + 1, InvalidId);
    }
    const auto newId                    = freeIds.allocate();
    textureIdToMaterialId[texture.id()] = newId;
    materials[newId]                    = mat::Material(texture, texture, defaultNormalMap);
    markForUpdate(newId);
    return MaterialRef(this, newId);
}

MaterialRef MaterialPool::getOrCreateFromDiffuseAndSpecular(const TextureRef& diffuse,
                                                            const TextureRef& specular) {
    std::unique_lock lock(mutex);

    checkLazyInit();

    const auto key = std::make_pair(diffuse.id(), specular.id());
    const auto it  = diffuseSpecularToMaterialId.find(key);
    if (it != diffuseSpecularToMaterialId.end()) { return MaterialRef(this, it->second); }

    const auto newId                 = freeIds.allocate();
    diffuseSpecularToMaterialId[key] = newId;
    materials[newId]                 = mat::Material(diffuse, specular, defaultNormalMap);
    markForUpdate(newId);
    return MaterialRef(this, newId);
}

void MaterialPool::init(vk::PerFrame<VkDescriptorSet>& descriptorSets,
                        vk::PerFrame<VkDescriptorSet>& rtDescriptorSets) {
    materials.resize(MaxMaterialCount);
    gpuPool.create(renderer.vulkanState(), MaxMaterialCount);
    refCounts.resize(MaxMaterialCount, 0);

    VkDescriptorBufferInfo bufferInfo;
    bufferInfo.buffer = gpuPool.gpuBufferHandle().getBuffer();
    bufferInfo.offset = 0;
    bufferInfo.range  = gpuPool.totalAlignedSize();

    std::array<VkWriteDescriptorSet, Config::MaxConcurrentFrames * 2> setWrites{};
    unsigned int si    = 0;
    const auto visitor = [&bufferInfo, &si, &setWrites](VkDescriptorSet set) {
        auto& write           = setWrites[si++];
        write.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorCount = 1;
        write.dstBinding      = DescriptorSetBindIndex;
        write.dstArrayElement = 0;
        write.dstSet          = set;
        write.pBufferInfo     = &bufferInfo;
        write.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    };
    descriptorSets.visit(visitor);
    rtDescriptorSets.visit(visitor);
    vkUpdateDescriptorSets(
        renderer.vulkanState().device, setWrites.size(), setWrites.data(), 0, nullptr);
}

void MaterialPool::checkLazyInit() {
    if (!defaultsInitialized) {
        defaultsInitialized = true;

        normalImage.create(2, 2, sf::Color(0, 0, 255));
        defaultNormalMap = renderer.texturePool().createTexture(
            normalImage, renderer.vulkanState().samplerCache.filteredRepeated());
    }
}

void MaterialPool::cleanup() { gpuPool.destroy(); }

VkDescriptorSetLayoutBinding MaterialPool::getLayoutBinding() const {
    VkDescriptorSetLayoutBinding binding{};
    binding.descriptorCount    = 1;
    binding.binding            = DescriptorSetBindIndex;
    binding.stageFlags         = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    binding.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    binding.pImmutableSamplers = 0;
    return binding;
}

void MaterialPool::release(std::uint32_t i) {
    std::unique_lock lock(mutex);

    freeIds.release(i);
    for (auto& mid : textureIdToMaterialId) {
        if (mid == i) { mid = InvalidId; }
    }

    markForUpdate(i);
}

void MaterialPool::markForUpdate(std::uint32_t i) {
    std::unique_lock lock(syncQueueMutex);
    toSync.emplace_back(i);
}

void MaterialPool::onFrameStart() {
    std::unique_lock lock(syncQueueMutex);
    if (!toSync.empty()) {
        std::unique_lock lock(mutex);
        for (std::uint32_t i : toSync) {
            auto& d             = gpuPool[i];
            auto& m             = materials[i];
            d.diffuseTextureId  = m.getTexture().id();
            d.normalTextureId   = m.getNormalMap().id();
            d.specularTextureId = m.getSpecularMap().id();
            d.shininess         = m.getShininess();
        }
        toSync.clear();
        gpuPool.queueTransfer(tfr::Transferable::SyncRequirement::Immediate);
    }
}

} // namespace res
} // namespace rc
} // namespace bl
