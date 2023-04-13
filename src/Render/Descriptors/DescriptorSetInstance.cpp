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
, staticChanged(false) {}

void DescriptorSetInstance::init(std::uint32_t ms, std::uint32_t md) {
    maxStatic = ms;
    doInit(ms, md);
}

bool DescriptorSetInstance::allocateObject(std::uint32_t sceneId, ecs::Entity entity,
                                           SceneObject::UpdateSpeed updateSpeed) {
    staticChanged = staticChanged || sceneId < maxStatic;
    return doAllocateObject(sceneId, entity, updateSpeed);
}

void DescriptorSetInstance::handleFrameStart() {
    beginSync(staticChanged);
    staticChanged = false;
}

} // namespace ds
} // namespace render
} // namespace bl
