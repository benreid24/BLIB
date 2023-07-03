#include <BLIB/Render/Descriptors/DescriptorSetInstance.hpp>

#include <limits>

namespace bl
{
namespace gfx
{
namespace ds
{
DescriptorSetInstance::DescriptorSetInstance(BindMode bindMode, SpeedBucketSetting speedSetting)
: bindless(bindMode == Bindless)
, speedBind(speedSetting == RebindForNewSpeed) {
    dirtyStatic.size   = 0;
    dirtyStatic.start  = std::numeric_limits<std::uint32_t>::max();
    dirtyDynamic.size  = 0;
    dirtyDynamic.start = std::numeric_limits<std::uint32_t>::max();
}

bool DescriptorSetInstance::allocateObject(ecs::Entity entity, scene::Key key) {
    markObjectDirty(key);
    return doAllocateObject(entity, key);
}

void DescriptorSetInstance::handleFrameStart() {
    dirtyStatic.size =
        dirtyStatic.size >= dirtyStatic.start ? dirtyStatic.size - dirtyStatic.start + 1 : 0;
    dirtyDynamic.size =
        dirtyDynamic.size >= dirtyDynamic.start ? dirtyDynamic.size - dirtyDynamic.start + 1 : 0;

    beginSync(dirtyStatic, dirtyDynamic);

    dirtyStatic.size   = 0;
    dirtyStatic.start  = std::numeric_limits<std::uint32_t>::max();
    dirtyDynamic.size  = 0;
    dirtyDynamic.start = std::numeric_limits<std::uint32_t>::max();
}

} // namespace ds
} // namespace gfx
} // namespace bl
