#include <BLIB/Render/Descriptors/Builtin/CommonSceneDescriptorSetInstance.hpp>

namespace bl
{
namespace render
{
namespace ds
{
CommonSceneDescriptorSetInstance::CommonSceneDescriptorSetInstance(VkDescriptorSetLayout layout)
: setLayout(layout) {}

CommonSceneDescriptorSetInstance::~CommonSceneDescriptorSetInstance() {
    viewProjBuf.stopTransferringEveryFrame();
    // TODO - cleanup
}

void CommonSceneDescriptorSetInstance::bindForPipeline(VkCommandBuffer commandBuffer,
                                                       VkPipelineLayout layout,
                                                       std::uint32_t observerIndex,
                                                       std::uint32_t setIndex) const {
    // TODO
}

void CommonSceneDescriptorSetInstance::bindForObject(VkCommandBuffer commandBuffer,
                                                     VkPipelineLayout layout,
                                                     std::uint32_t setIndex,
                                                     std::uint32_t objectId) const {
    // n/a
}

void CommonSceneDescriptorSetInstance::releaseObject(std::uint32_t sceneId, ecs::Entity entity) {
    // n/a
}

void CommonSceneDescriptorSetInstance::doInit(std::uint32_t maxStaticObjects,
                                              std::uint32_t maxDynamicObjects) {
    // TODO - create buffers + descriptors

    viewProjBuf.transferEveryFrame(tfr::Transferable::SyncRequirement::Immediate);
}

bool CommonSceneDescriptorSetInstance::doAllocateObject(std::uint32_t sceneId, ecs::Entity entity,
                                                        SceneObject::UpdateSpeed updateSpeed) {
    // n/a
    return false;
}

void CommonSceneDescriptorSetInstance::beginSync(bool) {
    viewProjBuf.write(observerCameras.data(), 0, observerCount());
}

} // namespace ds
} // namespace render
} // namespace bl
