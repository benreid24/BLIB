#ifndef BLIB_RENDER_DESCRIPTORS_COMMONSCENEDESCRIPTORSETINSTANCE_HPP
#define BLIB_RENDER_DESCRIPTORS_COMMONSCENEDESCRIPTORSETINSTANCE_HPP

#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Descriptors/SceneDescriptorSetInstance.hpp>
#include <BLIB/Render/Primitives/GenericBuffer.hpp>
#include <BLIB/Render/Util/PerFrame.hpp>
#include <BLIB/Render/Vulkan/DescriptorPool.hpp>
#include <array>
#include <glm/glm.hpp>

namespace bl
{
namespace render
{
namespace ds
{
class CommonSceneDescriptorSetInstance : public SceneDescriptorSetInstance {
public:
    CommonSceneDescriptorSetInstance(VkDescriptorSetLayout layout);

    virtual ~CommonSceneDescriptorSetInstance();

private:
    const VkDescriptorSetLayout setLayout;
    prim::GenericBuffer<glm::mat4, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, true> viewProjBuf;
    std::array<PerFrame<VkDescriptorSet>, Config::MaxSceneObservers> descriptorSets;
    std::array<DescriptorPool::AllocationHandle, Config::MaxSceneObservers> descriptorHandles;

    virtual void bindForPipeline(VkCommandBuffer commandBuffer, VkPipelineLayout layout,
                                 std::uint32_t observerIndex,
                                 std::uint32_t setIndex) const override;
    virtual void bindForObject(VkCommandBuffer commandBuffer, VkPipelineLayout layout,
                               std::uint32_t setIndex, std::uint32_t objectId) const override;
    virtual void releaseObject(std::uint32_t sceneId, ecs::Entity entity) override;
    virtual void doInit(std::uint32_t maxStaticObjects, std::uint32_t maxDynamicObjects) override;
    virtual bool doAllocateObject(std::uint32_t sceneId, ecs::Entity entity,
                                  SceneObject::UpdateSpeed updateSpeed) override;
    virtual void beginSync(bool staticObjectsChanged) override;
};

} // namespace ds
} // namespace render
} // namespace bl

#endif
