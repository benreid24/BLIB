#include <BLIB/Render/Descriptors/DescriptorSetInstance.hpp>

namespace bl
{
namespace render
{
namespace ds
{
DescriptorSetInstance::DescriptorSetInstance(bool po)
: perObject(po)
, maxStatic(0)
, staticChanged(0) {}

void DescriptorSetInstance::init(std::uint32_t ms, std::uint32_t md) {
    maxStatic = ms;
    doInit(ms, md);
}

bool DescriptorSetInstance::allocateObject(std::uint32_t sceneId, ecs::Entity entity,
                                           UpdateSpeed updateSpeed) {
    markObjectDirty(sceneId);
    return doAllocateObject(sceneId, entity, updateSpeed);
}

void DescriptorSetInstance::handleFrameStart() {
    beginSync(staticChanged > 0);
    staticChanged = staticChanged >> 1; // 2 -> 1 -> 0 -> 0 ...
}

} // namespace ds
} // namespace render
} // namespace bl
