#ifndef BLIB_RENDER_RESOURCES_MATERIALPOOL_HPP
#define BLIB_RENDER_RESOURCES_MATERIALPOOL_HPP

#include <BLIB/Containers/RefPoolDirect.hpp>
#include <BLIB/Render/Buffers/StaticUniformBuffer.hpp>
#include <BLIB/Render/Materials/Material.hpp>
#include <BLIB/Render/Materials/MaterialDescriptor.hpp>
#include <BLIB/Render/Resources/MaterialRef.hpp>
#include <BLIB/Render/Vulkan/PerFrame.hpp>
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
     * @brief Creates or returns an existing material from a texture
     *
     * @param texture The texture the material should use
     * @return A ref to the material
     */
    MaterialRef getOrCreateFromTexture(const res::TextureRef& texture);

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

    // indexes
    std::vector<std::uint32_t> textureIdToMaterialId;

    MaterialPool(Renderer& renderer);
    void init(vk::PerFrame<VkDescriptorSet>& descriptorSets,
              vk::PerFrame<VkDescriptorSet>& rtDescriptorSets);
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
