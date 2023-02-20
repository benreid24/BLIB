#ifndef BLIB_RENDER_RENDERER_TEXTUREPOOL_HPP
#define BLIB_RENDER_RENDERER_TEXTUREPOOL_HPP

#include <BLIB/Render/Renderer/Texture.hpp>
#include <BLIB/Render/Resources/TextureRef.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>
#include <BLIB/Util/IdAllocator.hpp>
#include <SFML/Graphics/Image.hpp>
#include <array>
#include <atomic>
#include <cstdint>
#include <glad/vulkan.h>
#include <glm/glm.hpp>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace bl
{
namespace render
{
class TexturePool {
public:
    static constexpr std::uint32_t MaxTextureCount       = 4096;
    static constexpr std::uint32_t TextureArrayBindIndex = 0;

    constexpr VkDescriptorSetLayout getDescriptorLayout() const;

    constexpr VkDescriptorSet getDescriptorSet() const;

    void bindDescriptors(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout,
                         std::uint32_t setIndex = 0);

    TextureRef createTexture(glm::u32vec2 size);

    TextureRef createTexture(const sf::Image& image);

    TextureRef getOrLoadTexture(const std::string& filePath);

    void releaseUnused();

private:
    std::mutex mutex;
    VulkanState& vulkanState;
    sf::Image errorPattern;
    Texture errorTexture;
    std::vector<Texture> textures;
    std::vector<std::atomic<std::uint32_t>> refCounts;
    util::IdAllocator<std::uint32_t> freeSlots;
    std::unordered_map<std::string, std::uint32_t> fileMap;
    std::vector<const std::string*> reverseFileMap;
    std::vector<std::uint32_t> toRelease;

    VkDescriptorPool descriptorPool;
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorSet descriptorSet;

    TexturePool(VulkanState& vulkanState);
    void init();
    void cleanup();

    TextureRef allocateTexture();
    void finalizeNewTexture(std::uint32_t i);

    void writeAllDescriptors();
    void writeDescriptor(std::uint32_t index);
    void queueForRelease(std::uint32_t i);
    void releaseUnusedLocked();

    friend class TextureRef;
    friend class Renderer;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr VkDescriptorSetLayout TexturePool::getDescriptorLayout() const {
    return descriptorSetLayout;
}

inline constexpr VkDescriptorSet TexturePool::getDescriptorSet() const { return descriptorSet; }

} // namespace render
} // namespace bl

#endif
