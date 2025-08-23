#ifndef BLIB_RENDER_RENDERER_TEXTUREPOOL_HPP
#define BLIB_RENDER_RENDERER_TEXTUREPOOL_HPP

#include <BLIB/Models/Texture.hpp>
#include <BLIB/Render/Descriptors/SetWriteHelper.hpp>
#include <BLIB/Render/Resources/TextureRef.hpp>
#include <BLIB/Render/Vulkan/CommonTextureFormats.hpp>
#include <BLIB/Render/Vulkan/Sampler.hpp>
#include <BLIB/Render/Vulkan/Texture.hpp>
#include <BLIB/Render/Vulkan/TextureOptions.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>
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
    static constexpr std::uint32_t CubemapArrayBindIndex = 3;
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
     *        the given resource id. If a texture for the given resource id already exists then it
     *        is returned immediately.
     *
     * @param filePath The resource id to load the contents from
     * @param options The texture options
     * @return A ref to the new or existing texture
     */
    TextureRef getOrLoadTexture(const std::string& filePath,
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
     * @brief Gets or creates a texture from the given model texture
     *
     * @param texture The source model texture
     * @param fallback The fallback texture to use if loading fails
     * @param options The texture options
     * @return A ref to the new or existing texture
     */
    TextureRef getOrCreateTexture(const mdl::Texture& texture, TextureRef fallback = {},
                                  const vk::TextureOptions& options = {});

    /**
     * @brief Creates a cubemap texture from the given faces
     *
     * @param right The right face image
     * @param left The left face image
     * @param top The top face image
     * @param bottom The bottom face image
     * @param back The back face image
     * @param front The front face image
     * @param format The format of the texture
     * @param sampler The sampler to use
     * @return A ref to the new cubemap texture
     */
    TextureRef createCubemap(
        const std::string& right, const std::string& left, const std::string& top,
        const std::string& bottom, const std::string& back, const std::string& front,
        VkFormat format                  = vk::CommonTextureFormats::SRGBA32Bit,
        vk::SamplerOptions::Type sampler = vk::SamplerOptions::Type::FilteredEdgeClamped);

    /**
     * @brief Creates a cubemap texture from the given faces
     *
     * @param right The right face image
     * @param left The left face image
     * @param top The top face image
     * @param bottom The bottom face image
     * @param back The back face image
     * @param front The front face image
     * @param format The format of the texture
     * @param sampler The sampler to use
     * @return A ref to the new cubemap texture
     */
    TextureRef createCubemap(
        resource::Ref<sf::Image> right, resource::Ref<sf::Image> left, resource::Ref<sf::Image> top,
        resource::Ref<sf::Image> bottom, resource::Ref<sf::Image> back,
        resource::Ref<sf::Image> front, VkFormat format = vk::CommonTextureFormats::SRGBA32Bit,
        vk::SamplerOptions::Type sampler = vk::SamplerOptions::Type::FilteredEdgeClamped);

    /**
     * @brief Creates a cubemap texture from the given packed texture. Should be packed according to
     *        VK_TEXTURE_CUBE_MAP_{face} order in a row
     *
     * @param packed The packed image to use
     * @param format The format of the texture
     * @param sampler The sampler to use
     * @return A ref to the new cubemap texture
     */
    TextureRef createCubemap(
        resource::Ref<sf::Image> packed, VkFormat format = vk::CommonTextureFormats::SRGBA32Bit,
        vk::SamplerOptions::Type sampler = vk::SamplerOptions::Type::FilteredEdgeClamped);

    /**
     * @brief Creates a cubemap texture from the given packed texture. Should be packed according to
     *        VK_TEXTURE_CUBE_MAP_{face} order in a row
     *
     * @param packed The packed image to use
     * @param format The format of the texture
     * @param sampler The sampler to use
     * @return A ref to the new cubemap texture
     */
    TextureRef getOrCreateCubemap(
        const std::string& packed, VkFormat format = vk::CommonTextureFormats::SRGBA32Bit,
        vk::SamplerOptions::Type sampler = vk::SamplerOptions::Type::FilteredEdgeClamped);

    /**
     * @brief Creates a cubemap texture from the given packed texture. Should be packed according to
     *        VK_TEXTURE_CUBE_MAP_{face} order in a row
     *
     * @param packed The packed image to use. Will be copied to local storage
     * @param format The format of the texture
     * @param sampler The sampler to use
     * @return A ref to the new cubemap texture
     */
    TextureRef createCubemap(
        const sf::Image& packed, VkFormat format = vk::CommonTextureFormats::SRGBA32Bit,
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
    vk::VulkanState& vulkanState;

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
    std::unordered_map<std::string, std::uint32_t> fileMap;
    std::unordered_map<const sf::Image*, std::uint32_t> imageMap;
    std::vector<const std::string*> reverseFileMap;
    std::vector<const sf::Image*> reverseImageMap;
    std::unordered_map<std::string, std::uint32_t> cubemapFileMap;
    std::vector<const std::string*> cubeMapReverseFileMap;

    // dynamics
    std::vector<vk::Texture*> toRelease;
    vk::PerFrame<std::vector<vk::Texture*>> queuedUpdates;

    TexturePool(Renderer& renderer, vk::VulkanState& vulkanState);
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
    void prepareTextureUpdate(vk::Texture* texture, const std::string& path);

    friend class TextureRef;
    friend class bl::rc::Renderer;
    friend class GlobalDescriptors;
    friend class vk::Texture;
};

} // namespace res
} // namespace rc
} // namespace bl

#endif
