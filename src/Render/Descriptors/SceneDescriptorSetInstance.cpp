#include <BLIB/Render/Descriptors/SceneDescriptorSetInstance.hpp>

#include <BLIB/Render/Config.hpp>

namespace bl
{
namespace rc
{
namespace ds
{
SceneDescriptorSetInstance::SceneDescriptorSetInstance()
: DescriptorSetInstance(Bindless, SpeedAgnostic) {}

void SceneDescriptorSetInstance::updateObserverCamera(std::uint32_t observerIndex,
                                                      const glm::mat4& cam) {
    cameraBuffer[observerIndex] = cam;
}

} // namespace ds
} // namespace rc
} // namespace bl
