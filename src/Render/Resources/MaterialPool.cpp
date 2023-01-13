#include <BLIB/Render/Resources/MaterialPool.hpp>

namespace bl
{
namespace render
{
MaterialPool::MaterialPool(VulkanState& vs)
: vulkanState(vs)
, freeSlots(MaxMaterialCount) {
    // TODO
}

MaterialPool::~MaterialPool() {
    // TODO
}

// TODO

} // namespace render
} // namespace bl
