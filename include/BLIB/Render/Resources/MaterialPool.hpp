#ifndef BLIB_RENDER_MATERIALCACHE_HPP
#define BLIB_RENDER_MATERIALCACHE_HPP

#include <BLIB/Render/Renderer/Material.hpp>
#include <BLIB/Render/Uniforms/MaterialUniform.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>
#include <BLIB/Util/IdAllocator.hpp>
#include <array>
#include <cstdint>
#include <glad/vulkan.h>
#include <string>
#include <unordered_map>

namespace bl
{
namespace render
{
class MaterialPool {
public:
    static constexpr std::uint32_t MaxMaterialCount = 4096;

    MaterialPool(VulkanState& vulkanState);

    ~MaterialPool();

    VkDescriptorSetLayout getDescriptorLayout();

    void bindDescriptors(std::uint32_t setIndex);

    std::uint32_t createMaterial(Material&& material);

    std::uint32_t getOrLoadMaterial(const std::string& materialFile);

    std::uint32_t getOrLoadMaterialFromTexture(const std::string& textureFile);

    Material* retrieveMaterial(std::uint32_t materialId);

    void syncMaterial(std::uint32_t materialId);

    void syncAll();

    // TODO - lifetime management

private:
    VulkanState& vulkanState;
    std::vector<Material> materials;
    std::vector<MaterialUniform> materialUniforms;
    util::IdAllocator<std::size_t> freeSlots;

    std::unordered_map<std::string, std::uint32_t> materialFileMap;
    std::unordered_map<std::string, std::uint32_t> textureFileMap;

    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorSet descriptorSet;
    VkWriteDescriptorSet descriptorUpdateCommand;

    std::vector<VkDescriptorImageInfo> textureImageWriteInfo;
    VkBuffer materialDeviceBuffer;

    void resetMaterial(std::uint32_t materialId);
};

} // namespace render
} // namespace bl

#endif
