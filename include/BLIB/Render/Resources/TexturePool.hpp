#ifndef BLIB_RENDER_RENDERER_TEXTUREPOOL_HPP
#define BLIB_RENDER_RENDERER_TEXTUREPOOL_HPP

#include <BLIB/Render/Resources/BindlessTextureArray.hpp>
#include <BLIB/Render/Resources/TextureRef.hpp>
#include <BLIB/Render/Vulkan/Texture.hpp>
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
 * @brief Resource manager for textures on the GPU. Utilizes BindlessTextureArray to provide a large
 *        array, in a single descriptor set, of Sampler2D instances for each texture
 *
 * @ingroup Renderer
 */
class TexturePool {
public:
    static constexpr std::uint32_t TextureArrayBindIndex = 0;

    /**
     * @brief Creates an empty texture of the given size
     *
     * @param size The size of the texture to create
     * @param sampler The sampler to use
     * @return A reference to the new texture
     */
    TextureRef createTexture(const glm::u32vec2& size, VkSampler sampler = nullptr);

    /**
     * @brief Creates an empty texture of the given size to be rendered to
     *
     * @param size The size of the texture to create
     * @param sampler The sampler to use
     * @return A reference to the new texture
     */
    TextureRef createRenderTexture(const glm::u32vec2& size, VkSampler sampler = nullptr);

    /**
     * @brief Creates a new texture from the given contents and sampler
     *
     * @param image The contents to fill the texture with
     * @param sampler The sampler the texture should use
     * @return A reference to the new texture
     */
    TextureRef createTexture(const sf::Image& image, VkSampler sampler = nullptr);

    /**
     * @brief Potentially creates a new texture and returns it. The texture contents are loaded from
     *        the given resource id. If a texture for the given resource id already exists then it
     *        is returned immediately.
     *
     * @param filePath The resource id to load the contents from
     * @param sampler The sampler to use
     * @return A ref to the new or existing texture
     */
    TextureRef getOrLoadTexture(const std::string& filePath, VkSampler sampler = nullptr);

    /**
     * @brief Potentially creates a new texture and returns it. The texture contents are loaded from
     *        the given image. If a texture for the given image already exists then it
     *        is returned immediately.
     *
     * @param src The source image to load
     * @param sampler The sampler to use
     * @return A ref to the new or existing texture
     */
    TextureRef getOrLoadTexture(const sf::Image& src, VkSampler sampler = nullptr);

    /**
     * @brief Frees all textures that no longer have any valid refs pointing to them
     */
    void releaseUnused();

    /**
     * @brief Manually releases the given texture, even if other locations are still pointing to it
     *
     * @param ref The texture to release
     */
    void releaseTexture(const TextureRef& ref);

    /**
     * @brief Returns a shared blank texture. Do not modify the blank texture
     */
    TextureRef getBlankTexture();

    /**
     * @brief Returns a layout binding to be used for descriptor set layout creation
     */
    VkDescriptorSetLayoutBinding getLayoutBinding() const;

private:
    std::mutex mutex;
    vk::VulkanState& vulkanState;
    BindlessTextureArray textures;
    std::vector<std::atomic<std::uint32_t>> refCounts;
    util::IdAllocator<std::uint32_t> freeSlots;
    util::IdAllocator<std::uint32_t> freeRtSlots;
    std::unordered_map<std::string, std::uint32_t> fileMap;
    std::unordered_map<const sf::Image*, std::uint32_t> imageMap;
    std::vector<const std::string*> reverseFileMap;
    std::vector<const sf::Image*> reverseImageMap;
    std::vector<std::uint32_t> toRelease;
    TextureRef blankTexture;

    TexturePool(vk::VulkanState& vulkanState);
    void init(vk::PerFrame<VkDescriptorSet>& descriptorSets,
              vk::PerFrame<VkDescriptorSet>& rtDescriptorSets);
    void cleanup();
    void onFrameStart(ds::SetWriteHelper& setWriter, VkDescriptorSet currentSet,
                      VkDescriptorSet currentRtSet);

    TextureRef allocateTexture();
    void queueForRelease(std::uint32_t i);
    void releaseUnusedLocked();
    void doRelease(std::uint32_t i);

    friend class TextureRef;
    friend class bl::rc::Renderer;
    friend class GlobalDescriptors;
};

} // namespace res
} // namespace rc
} // namespace bl

#endif
