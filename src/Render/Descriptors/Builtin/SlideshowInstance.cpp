#include <BLIB/Render/Descriptors/Builtin/SlideshowInstance.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace ds
{
SlideshowInstance::SlideshowInstance(engine::Engine& engine,
                                     VkDescriptorSetLayout descriptorSetLayout)
: DescriptorSetInstance(Bindless, SpeedAgnostic)
, registry(engine.ecs())
, vulkanState(engine.renderer().vulkanState())
, descriptorSetLayout(descriptorSetLayout) {}

void SlideshowInstance::init(DescriptorComponentStorageCache& storageCache) {
    // TODO
}

void SlideshowInstance::bindForPipeline(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                                        std::uint32_t setIndex, UpdateSpeed updateFreq) const {
    // TODO
}

void SlideshowInstance::bindForObject(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                                      std::uint32_t setIndex, scene::Key objectKey) const {
    // TODO
}

bool SlideshowInstance::allocateObject(ecs::Entity entity, scene::Key key) {
    // TODO
    return false;
}

void SlideshowInstance::releaseObject(ecs::Entity entity, scene::Key key) {
    // TODO
}

void SlideshowInstance::handleFrameStart() {
    // TODO
}

} // namespace ds
} // namespace rc
} // namespace bl
