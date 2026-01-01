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
sf::Image parallaxImage;
} // namespace

MaterialPool::MaterialPool(Renderer& renderer)
: renderer(renderer)
, freeIds(MaxMaterialCount) {}

MaterialRef MaterialPool::create(const TextureRef& diffuse, const TextureRef& specular,
                                 const TextureRef& normal, const TextureRef& parallax,
                                 float heightScale, float shininess) {
    std::unique_lock lock(mutex);

    const auto newId = freeIds.allocate();
    materials[newId] = mat::Material(diffuse, specular, normal, parallax, heightScale, shininess);
    markForUpdate(newId);
    return MaterialRef(this, newId);
}

MaterialRef MaterialPool::getOrCreateFromTexture(const res::TextureRef& texture) {
    std::unique_lock lock(mutex);

    checkLazyInit();

    const bool canCache = texture->getType() != vk::Texture::Type::Cubemap;

    if (texture.id() < textureIdToMaterialId.size() &&
        textureIdToMaterialId[texture.id()] != InvalidId && canCache) {
        return MaterialRef(this, textureIdToMaterialId[texture.id()]);
    }

    const auto newId = freeIds.allocate();
    if (canCache) {
        if (textureIdToMaterialId.size() <= texture.id()) {
            textureIdToMaterialId.resize(texture.id() + 1, InvalidId);
        }
        textureIdToMaterialId[texture.id()] = newId;
    }
    materials[newId] = mat::Material(texture, texture, defaultNormalMap, defaultParallaxMap);
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
    materials[newId] = mat::Material(diffuse, specular, defaultNormalMap, defaultParallaxMap);
    markForUpdate(newId);
    return MaterialRef(this, newId);
}

void MaterialPool::init(vk::PerFrame<VkDescriptorSet>& descriptorSets,
                        vk::PerFrame<VkDescriptorSet>& rtDescriptorSets) {
    materials.resize(MaxMaterialCount);
    gpuPool.create(renderer.vulkanState(), MaxMaterialCount);
    refCounts.resize(MaxMaterialCount, 0);

    VkDescriptorBufferInfo bufferInfo;
    bufferInfo.buffer = gpuPool.getCurrentFrameRawBuffer();
    bufferInfo.offset = 0;
    bufferInfo.range  = gpuPool.getTotalAlignedSize();

    std::array<VkWriteDescriptorSet, cfg::Limits::MaxConcurrentFrames * 2> setWrites{};
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
        renderer.vulkanState().getDevice(), setWrites.size(), setWrites.data(), 0, nullptr);
}

void MaterialPool::checkLazyInit() {
    if (!defaultsInitialized) {
        defaultsInitialized = true;

        normalImage.create(2, 2, sf::Color(128, 128, 255));
        defaultNormalMap = renderer.texturePool().createTexture(
            normalImage,
            {.format  = vk::CommonTextureFormats::LinearRGBA32Bit,
             .sampler = vk::SamplerOptions::Type::FilteredRepeated});

        parallaxImage.create(2, 2, sf::Color::Black);
        defaultParallaxMap = renderer.texturePool().createTexture(
            parallaxImage,
            {.format  = vk::CommonTextureFormats::LinearRGBA32Bit,
             .sampler = vk::SamplerOptions::Type::FilteredRepeated});
    }
}

void MaterialPool::cleanup() { gpuPool.destroy(); }

MaterialRef MaterialPool::getOrCreateFromNormalAndParallax(const TextureRef& diffuse,
                                                           const TextureRef& normal,
                                                           const TextureRef& parallax,
                                                           float heightScale) {
    std::unique_lock lock(mutex);

    checkLazyInit();

    const auto key = std::make_pair(diffuse.id(), std::make_pair(normal.id(), parallax.id()));
    const auto it  = normalParallaxToMaterialId.find(key);
    if (it != normalParallaxToMaterialId.end()) { return MaterialRef(this, it->second); }

    const auto newId                = freeIds.allocate();
    normalParallaxToMaterialId[key] = newId;
    materials[newId] = mat::Material(diffuse, diffuse, normal, parallax, heightScale);
    markForUpdate(newId);
    return MaterialRef(this, newId);
}

MaterialRef MaterialPool::getOrCreateFromModelMaterial(const mdl::Material& src) {
    std::unique_lock lock(mutex);

    checkLazyInit();

    const auto it = modelMaterialToId.find(src);
    if (it != modelMaterialToId.end()) { return MaterialRef(this, it->second); }

    auto diffuse = renderer.texturePool().getOrCreateTexture(
        src.diffuse,
        {},
        {.format  = vk::CommonTextureFormats::SRGBA32Bit,
         .sampler = vk::SamplerOptions::Type::FilteredRepeated});
    auto specular = renderer.texturePool().getOrCreateTexture(
        src.specular,
        diffuse,
        {.format  = vk::CommonTextureFormats::LinearRGBA32Bit,
         .sampler = vk::SamplerOptions::Type::FilteredRepeated});
    auto normal = renderer.texturePool().getOrCreateTexture(
        src.normal,
        defaultNormalMap,
        {.format  = vk::CommonTextureFormats::LinearRGBA32Bit,
         .sampler = vk::SamplerOptions::Type::FilteredRepeated});
    auto parallax = renderer.texturePool().getOrCreateTexture(
        src.parallax,
        defaultParallaxMap,
        {.format  = vk::CommonTextureFormats::LinearRGBA32Bit,
         .sampler = vk::SamplerOptions::Type::FilteredRepeated});

    const auto newId = freeIds.allocate();
    materials[newId] =
        mat::Material(diffuse, specular, normal, parallax, src.heightScale, src.shininess);

    modelMaterialToId[src] = newId;
    markForUpdate(newId);
    return MaterialRef(this, newId);
}

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
            d.parallaxTextureId = m.getParallaxMap().id();
            d.heightScale       = m.getHeightScale();
            d.shininess         = m.getShininess();

            gpuPool.markDirty(i);
        }
        toSync.clear();
    }
}

TextureRef MaterialPool::getDefaultNormalMap() {
    checkLazyInit();
    return defaultNormalMap;
}

TextureRef MaterialPool::getDefaultParallaxMap() {
    checkLazyInit();
    return defaultParallaxMap;
}

} // namespace res
} // namespace rc
} // namespace bl
