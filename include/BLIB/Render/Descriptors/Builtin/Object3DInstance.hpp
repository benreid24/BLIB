#ifndef BLIB_RENDER_DESCRIPTORS_BUILTIN_OBJECT3DINSTANCE_HPP
#define BLIB_RENDER_DESCRIPTORS_BUILTIN_OBJECT3DINSTANCE_HPP

#include <BLIB/ECS/Registry.hpp>
#include <BLIB/Render/Buffers/DynamicSSBO.hpp>
#include <BLIB/Render/Buffers/StaticSSBO.hpp>
#include <BLIB/Render/Components/Texture.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetInstance.hpp>
#include <BLIB/Render/Vulkan/PerFrameVector.hpp>
#include <BLIB/Transforms/3D.hpp>
#include <cstdint>
#include <glm/glm.hpp>
#include <vector>

namespace bl
{
namespace engine
{
class Engine;
}

namespace gfx
{
class Renderer;

namespace ds
{
/**
 * @brief Descriptor set instance used by all meshes in the engine default pipelines. Contains the
 *        object transform matrix and texture id
 *
 * @ingroup Renderer
 */
class Object3DInstance : public DescriptorSetInstance {
public:
    /**
     * @brief Create a new set instance
     *
     * @param engine Game engine instance
     * @param descriptorSetLayout Layout of the descriptor set
     */
    Object3DInstance(engine::Engine& engine, VkDescriptorSetLayout descriptorSetLayout);

    /**
     * @brief Frees resources
     */
    virtual ~Object3DInstance();

private:
    ecs::Registry& registry;
    vk::VulkanState& vulkanState;
    vk::DescriptorPool::AllocationHandle alloc;
    const VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorSet allocatedSets[Config::MaxConcurrentFrames + 1];
    vk::PerFrame<VkDescriptorSet> dynamicDescriptorSets;
    ds::DescriptorComponentStorage<t3d::Transform3D, glm::mat4>* transforms;
    ds::DescriptorComponentStorage<com::Texture, std::uint32_t, buf::StaticSSBO<std::uint32_t>,
                                   buf::StaticSSBO<std::uint32_t>>* textures;

    virtual void bindForPipeline(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                                 std::uint32_t setIndex, UpdateSpeed updateFreq) const override;
    virtual void bindForObject(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                               std::uint32_t setIndex, scene::Key objectKey) const override;
    virtual void releaseObject(ecs::Entity entity, scene::Key objectKey) override;
    virtual void init(DescriptorComponentStorageCache& storageCache) override;
    virtual bool allocateObject(ecs::Entity entity, scene::Key key) override;
    virtual void handleFrameStart() override;

    void updateStaticDescriptors();
    void updateDynamicDescriptors();
};

} // namespace ds
} // namespace gfx
} // namespace bl

#endif
