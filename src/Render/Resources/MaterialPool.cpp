#include <BLIB/Render/Resources/MaterialPool.hpp>

namespace bl
{
namespace render
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

} // namespace render
} // namespace bl
