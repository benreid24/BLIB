#include <BLIB/Render/Resources/MaterialPool.hpp>

namespace bl
{
namespace rc
{
namespace res
{
MaterialPool::MaterialPool(vk::VulkanState& vs)
: vulkanState(vs)
, materials(MaxMaterialCount)
//, materialUniforms(MaxMaterialCount)
, freeSlots(MaxMaterialCount)
, textureImageWriteInfo(MaxMaterialCount) {}

MaterialPool::~MaterialPool() {
    // TODO
}

// TODO

} // namespace res
} // namespace rc
} // namespace bl
