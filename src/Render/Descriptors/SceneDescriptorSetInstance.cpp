#include <BLIB/Render/Descriptors/Builtin/Scene3DUnlitInstance.hpp>

#include <BLIB/Render/Config.hpp>

namespace bl
{
namespace render
{
namespace ds
{
SceneDescriptorSetInstance::SceneDescriptorSetInstance()
: DescriptorSetInstance(false) {}

void SceneDescriptorSetInstance::updateObserverCamera(std::uint32_t observerIndex,
                                                      const glm::mat4& cam) {
    cameraBuffer[observerIndex] = cam;
}

} // namespace ds
} // namespace render
} // namespace bl
