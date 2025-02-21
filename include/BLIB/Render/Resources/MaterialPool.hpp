#ifndef BLIB_RENDER_RESOURCES_MATERIALPOOL_HPP
#define BLIB_RENDER_RESOURCES_MATERIALPOOL_HPP

#include <BLIB/Containers/RefPoolDirect.hpp>
#include <BLIB/Render/Buffers/StaticUniformBuffer.hpp>
#include <BLIB/Render/Materials/Material.hpp>
#include <BLIB/Render/Materials/MaterialDescriptor.hpp>
#include <BLIB/Render/Resources/MaterialRef.hpp>
#include <BLIB/Render/Vulkan/PerFrame.hpp>
#include <BLIB/Util/Hashes.hpp>
#include <BLIB/Util/IdAllocator.hpp>
#include <BLIB/Vulkan.hpp>
#include <cstdint>
#include <mutex>

namespace bl
{
namespace rc
{
class Renderer;

namespace res
{
class GlobalDescriptors;

/**
 * @brief Collection of materials currently in use. Similar to TexturePool
 *
 * @ingroup Renderer
 */
class MaterialPool {
public:
    static constexpr unsigned int DescriptorSetBindIndex = 1;
    static constexpr std::uint32_t MaxMaterialCount      = 2048;

    /**
     * @brief Creates a new material from all of its parameters
     *
     * @param diffuse The diffuse texture
     * @param specular The specular map
     * @param normal The normal map
     * @param shininess The shininess
     * @return A reference to the new material
     */
    MaterialRef create(const TextureRef& diffuse, const TextureRef& specular,
                       const TextureRef& normal, float shininess);

    /**
     * @brief Creates or returns an existing material from a texture
     *
     * @param texture The texture the material should use
     * @return A ref to the material
     */
    MaterialRef getOrCreateFromTexture(const TextureRef& texture);

    /**
     * @brief Creates or returns an existing material from a diffuse and specular texture
     *
     * @param diffuse The diffuse texture the material should use
     * @param specular The specular texture the material should use
     * @return A ref to the material
     */
    MaterialRef getOrCreateFromDiffuseAndSpecular(const TextureRef& diffuse,
                                                  const TextureRef& specular);

    /**
     * @brief Returns a layout binding to be used for descriptor set layout creation
     */
    VkDescriptorSetLayoutBinding getLayoutBinding() const;

private:
    static constexpr std::uint32_t InvalidId = std::numeric_limits<std::uint32_t>::max();

    // core data
    std::mutex mutex;
    std::mutex syncQueueMutex;
    Renderer& renderer;

    // material data
    std::vector<mat::Material> materials;
    buf::StaticUniformBuffer<mat::MaterialDescriptor> gpuPool;

    // management data
    std::vector<std::uint32_t> refCounts;
    util::IdAllocator<std::uint32_t> freeIds;
    std::vector<std::uint32_t> toSync;

    // default maps
    TextureRef defaultNormalMap;

    // indexes
    std::vector<std::uint32_t> textureIdToMaterialId;
    std::unordered_map<std::pair<std::uint32_t, std::uint32_t>, std::uint32_t,
                       util::PairHash<std::uint32_t, std::uint32_t>>
        diffuseSpecularToMaterialId;

    MaterialPool(Renderer& renderer);
    void init(vk::PerFrame<VkDescriptorSet>& descriptorSets,
              vk::PerFrame<VkDescriptorSet>& rtDescriptorSets);
    void checkLazyInit();
    void cleanup();
    void onFrameStart();

    void release(std::uint32_t i);
    void markForUpdate(std::uint32_t i);

    friend class ::bl::rc::Renderer;
    friend class GlobalDescriptors;
    friend class MaterialRef;
};

} // namespace res
} // namespace rc
} // namespace bl

#endif
