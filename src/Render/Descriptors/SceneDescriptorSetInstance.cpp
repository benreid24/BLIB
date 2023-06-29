#include <BLIB/Render/Descriptors/SceneDescriptorSetInstance.hpp>

#include <BLIB/Render/Config.hpp>

namespace bl
{
namespace gfx
{
namespace ds
{
SceneDescriptorSetInstance::SceneDescriptorSetInstance()
: DescriptorSetInstance(true) {}

void SceneDescriptorSetInstance::updateObserverCamera(std::uint32_t observerIndex,
                                                      const glm::mat4& cam) {
    cameraBuffer[observerIndex] = cam;
}

} // namespace ds
} // namespace gfx
} // namespace bl
