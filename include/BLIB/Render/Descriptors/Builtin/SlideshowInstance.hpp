#ifndef BLIB_RENDER_DESCRIPTORS_BUILTIN_SLIDESHOWINSTANCE_HPP
#define BLIB_RENDER_DESCRIPTORS_BUILTIN_SLIDESHOWINSTANCE_HPP

#include <BLIB/Components/Animation2D.hpp>
#include <BLIB/Components/Animation2DPlayer.hpp>
#include <BLIB/ECS/ComponentPool.hpp>
#include <BLIB/Render/Buffers/DynamicSSBO.hpp>
#include <BLIB/Render/Buffers/StaticSSBO.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetInstance.hpp>
#include <BLIB/Render/Vulkan/DescriptorSet.hpp>

namespace bl
{
namespace rc
{
namespace ds
{
/**
 * @brief Provides slideshow data for animations to render
 *
 * @ingroup Renderer
 */
class SlideshowInstance : public DescriptorSetInstance {
public:
    /**
     * @brief Create a new set instance
     *
     * @param engine Game engine instance
     * @param descriptorSetLayout Layout of the descriptor set
     */
    SlideshowInstance(engine::Engine& engine, VkDescriptorSetLayout descriptorSetLayout);

    /**
     * @brief Destroys the descriptor set instance
     */
    virtual ~SlideshowInstance() = default;

private:
    struct Frame {
        glm::vec2 texCoords[4];
        float opacity;
    };

    // core data
    ecs::Registry& registry;
    vk::VulkanState& vulkanState;
    const VkDescriptorSetLayout descriptorSetLayout;
    vk::PerFrame<vk::DescriptorSet> staticDescriptorSets;
    vk::PerFrame<vk::DescriptorSet> dynamicDescriptorSets;

    // dirty state
    std::uint8_t refreshRequired;
    std::uint32_t lastFrameUploaded;
    std::uint32_t lastOffsetUploaded;

    // mappings
    std::unordered_map<ecs::Entity, std::uint32_t> playerIndices;
    std::uint32_t nextPlayerIndex;
    ecs::ComponentPool<com::Animation2D>* animComponents;

    // animation level data
    buf::StaticSSBO<Frame> framesSSBO;                      // all anim frames
    buf::StaticSSBO<std::uint32_t> frameOffsetSSBO;         // playerIndex -> frame index
    buf::DynamicSSBO<std::uint32_t> playerCurrentFrameSSBO; // playerIndex -> current frame

    // object level data
    DescriptorComponentStorage<com::Animation2D, std::uint32_t>* objToAnimIndex;

    virtual void init(DescriptorComponentStorageCache& storageCache) override;
    virtual void bindForPipeline(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                                 std::uint32_t setIndex, UpdateSpeed updateFreq) const override;
    virtual void bindForObject(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                               std::uint32_t setIndex, scene::Key objectKey) const override;
    virtual bool allocateObject(ecs::Entity entity, scene::Key key) override;
    virtual void releaseObject(ecs::Entity entity, scene::Key key) override;
    virtual void handleFrameStart() override;

    // populate frames and offset, return false on error. lookup before add
    bool ensurePlayerPresent(ecs::Entity player, std::uint32_t& playerIndex);
    void refreshPlayerPointers(); // call when playerCurrentFrameSSBO size changes
    void updateDescriptorSets();
};

} // namespace ds
} // namespace rc
} // namespace bl

#endif
