#ifndef BLIB_RENDER_RENDERER_TEXTUREPOOL_HPP
#define BLIB_RENDER_RENDERER_TEXTUREPOOL_HPP

#include <BLIB/Assets/Builtin/CubemapPayload.hpp>
#include <BLIB/Assets/Builtin/ImagePayload.hpp>
#include <BLIB/Assets/Builtin/TexturePayload.hpp>
#include <BLIB/Assets/TypedRef.hpp>
#include <BLIB/Models/Texture.hpp>
#include <BLIB/Render/Descriptors/SetWriteHelper.hpp>
#include <BLIB/Render/Resources/TextureRef.hpp>
#include <BLIB/Render/Vulkan/CommonTextureFormats.hpp>
#include <BLIB/Render/Vulkan/Sampler.hpp>
#include <BLIB/Render/Vulkan/Texture.hpp>
#include <BLIB/Render/Vulkan/TextureOptions.hpp>
#include <BLIB/Render/Vulkan/VulkanLayer.hpp>
#include <BLIB/Util/IdAllocator.hpp>
#include <BLIB/Vulkan.hpp>
#include <SFML/Graphics/Image.hpp>
#include <array>
#include <atomic>
#include <cstdint>
#include <glm/glm.hpp>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace bl
{
namespace rc
{
namespace res
{
class GlobalDescriptors;

/**
 * @brief Resource manager for textures on the GPU. Provides a large texture array, in a single
 *        descriptor set, of Sampler2D instances for each texture
 *
 * @ingroup Renderer
 */
class TexturePool {
public:
    static constexpr std::uint32_t TextureArrayBindIndex = 0;
    static constexpr std::uint32_t CubemapArrayBindIndex = 2;
    static constexpr std::uint32_t FirstRenderTextureId =
        cfg::Limits::MaxTextureCount - cfg::Limits::MaxRenderTextures;
    static constexpr std::uint32_t ErrorTextureId =
        cfg::Limits::MaxTextureCount - cfg::Limits::MaxRenderTextures - 1;

    /**
     * @brief Creates an empty texture of the given size
     *
     * @param size The size of the texture to create
     * @param options The texture options
     * @return A reference to the new texture
     */
    TextureRef createTexture(const glm::u32vec2& size, const vk::TextureOptions& options = {});

    /**
     * @brief Creates an empty texture of the given size to be rendered to
     *
     * @param size The size of the texture to create
     * @param format The format of the texture
     * @param sampler The sampler to use
     * @return A reference to the new texture
     */
    TextureRef createRenderTexture(
        const glm::u32vec2& size, VkFormat format = vk::CommonTextureFormats::SRGBA32Bit,
        vk::SamplerOptions::Type sampler = vk::SamplerOptions::Type::FilteredBorderClamped);

    /**
     * @brief Creates a new texture from the given contents and sampler
     *
     * @param image The contents to fill the texture with
     * @param options The texture options
     * @return A reference to the new texture
     */
    TextureRef createTexture(const sf::Image& image, const vk::TextureOptions& options = {});

    /**
     * @brief Potentially creates a new texture and returns it. The texture contents are loaded from
     *        the given asset. If a texture for the given asset already exists then it
     *        is returned immediately.
     *
     * @param imageAsset The asset to load the contents from
     * @param options The texture options
     * @return A ref to the new or existing texture
     */
    TextureRef getOrLoadTexture(as::TypedRef<asi::ImagePayload> imageAsset,
                                const vk::TextureOptions& options = {});

    /**
     * @brief Potentially creates a new texture and returns it. The texture contents are loaded from
     *        the given asset. If a texture for the given asset already exists then it
     *        is returned immediately.
     *
     * @param textureAsset The asset to load the contents from
     * @param options The texture options
     * @return A ref to the new or existing texture
     */
    TextureRef getOrLoadTexture(as::TypedRef<asi::TexturePayload> textureAsset,
                                const vk::TextureOptions& options = {});

    /**
     * @brief Potentially creates a new texture and returns it. The texture contents are loaded from
     *        the given image. If a texture for the given image already exists then it
     *        is returned immediately.
     *
     * @param src The source image to load
     * @param options The texture options
     * @return A ref to the new or existing texture
     */
    TextureRef getOrLoadTexture(const sf::Image& src, const vk::TextureOptions& options = {});

    /**
     * @brief Creates a cubemap texture from the given faces
     *
     * @param cubemap The cubemap asset to use
     * @param sampler The sampler to use
     * @return A ref to the new cubemap texture
     */
    TextureRef getOrCreateCubemap(
        as::TypedRef<asi::CubemapPayload> cubemap,
        vk::SamplerOptions::Type sampler = vk::SamplerOptions::Type::FilteredEdgeClamped);

    /**
     * @brief Frees all textures that no longer have any valid refs pointing to them
     */
    void releaseUnused();

    /**
     * @brief Manually releases the given texture, even if other locations are still pointing to it
     *
     * @param ref The texture to release
     */
    void releaseTexture(TextureRef& ref);

    /**
     * @brief Returns a shared blank texture. Do not modify the blank texture
     */
    TextureRef getBlankTexture();

    /**
     * @brief Returns the layout binding for the texture array
     */
    VkDescriptorSetLayoutBinding getTextureLayoutBinding() const;

    /**
     * @brief Returns the layout binding for the cubemap array
     */
    VkDescriptorSetLayoutBinding getCubemapLayoutBinding() const;

private:
    // functional data
    std::mutex mutex;
    Renderer& renderer;
    vk::VulkanLayer& vulkanState;

    // core data
    std::vector<vk::Texture> textures;
    std::vector<vk::Texture> cubemaps;
    sf::Image errorPattern;
    sf::Image errorPatternCube;
    vk::Texture errorTexture;
    vk::Texture errorCubemap;
    TextureRef blankTexture;

    // allocation
    std::vector<std::atomic<std::uint32_t>> refCounts;
    util::IdAllocator<std::uint32_t> freeSlots;
    util::IdAllocator<std::uint32_t> freeRtSlots;
    std::vector<std::atomic<std::uint32_t>> cubemapRefCounts;
    util::IdAllocator<std::uint32_t> cubemapFreeSlots;

    // indices
    std::unordered_map<util::UUID, std::uint32_t> assetMap;
    std::unordered_map<const sf::Image*, std::uint32_t> imageMap;
    std::unordered_map<util::UUID, std::uint32_t> cubemapAssetMap;
    std::vector<util::UUID> reverseAssetMap;
    std::vector<const sf::Image*> reverseImageMap;
    std::vector<util::UUID> reverseCubemapAssetMap;

    // dynamics
    std::vector<vk::Texture*> toRelease;
    vk::PerFrame<std::vector<vk::Texture*>> queuedUpdates;

    TexturePool(Renderer& renderer, vk::VulkanLayer& vulkanState);
    void init(vk::PerFrame<VkDescriptorSet>& descriptorSets,
              vk::PerFrame<VkDescriptorSet>& rtDescriptorSets);
    void cleanup();
    void onFrameStart(ds::SetWriteHelper& setWriter, VkDescriptorSet currentSet,
                      VkDescriptorSet currentRtSet);

    TextureRef allocateTexture();
    TextureRef allocateCubemap();
    void queueForRelease(vk::Texture* texture);
    void releaseUnusedLocked();
    void doRelease(vk::Texture* texture);
    void cancelRelease(vk::Texture* texture);

    void updateTexture(vk::Texture* texture);
    void resetTexture(vk::Texture* texture);
    void prepareTextureUpdate(vk::Texture* texture, const sf::Image& src);
    void prepareTextureUpdate(vk::Texture* texture, as::TypedRef<asi::ImagePayload> asset);

    friend class TextureRef;
    friend class bl::rc::Renderer;
    friend class GlobalDescriptors;
    friend class vk::Texture;
};

} // namespace res
} // namespace rc
} // namespace bl

#endif
