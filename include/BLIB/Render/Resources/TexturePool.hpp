#ifndef BLIB_RENDER_RENDERER_TEXTUREPOOL_HPP
#define BLIB_RENDER_RENDERER_TEXTUREPOOL_HPP

#include <BLIB/Render/Renderer/Texture.hpp>
#include <BLIB/Render/Vulkan/DescriptorPoolRequirements.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>
#include <BLIB/Util/IdAllocator.hpp>
#include <SFML/Graphics/Image.hpp>
#include <array>
#include <cstdint>
#include <glad/vulkan.h>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace bl
{
namespace render
{
class TexturePool {
public:
    static constexpr std::uint32_t MaxTextureCount = 4096;

    TexturePool(VulkanState& vulkanState);

    ~TexturePool();

    void addDescriptorPoolRequirements(DescriptorPoolRequirements& requirements) const;

    VkDescriptorSetLayout getDescriptorLayout();

    void bindDescriptors(std::uint32_t setIndex);

    std::uint32_t createTexture(glm::vec2 size);

    std::uint32_t createTexture(const sf::Image& image);

    std::uint32_t getOrLoadTexture(const std::string& filePath);

    Texture* retreiveTexture(std::uint32_t textureId);

private:
    VulkanState& vulkanState;
    std::vector<Texture> textures;
    util::IdAllocator<std::uint32_t> freeSlots;
    std::unordered_map<std::string, std::uint32_t> fileMap;

    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorSet descriptorSet;
    VkWriteDescriptorSet descriptorUpdateCommand;
};

} // namespace render
} // namespace bl

#endif
