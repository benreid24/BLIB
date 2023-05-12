#include <BLIB/Render/Descriptors/Builtin/CommonSceneDescriptorSetInstance.hpp>

#include <BLIB/Render/Config.hpp>

namespace bl
{
namespace render
{
namespace ds
{
SceneDescriptorSetInstance::SceneDescriptorSetInstance()
: DescriptorSetInstance(false)
, nextObserverIndex(0) {}

std::uint32_t SceneDescriptorSetInstance::registerObserver() {
#ifdef BLIB_DEBUG
    if (nextObserverIndex >= Config::MaxSceneObservers) {
        BL_LOG_CRITICAL << "Max observer count for scene reached";
        throw std::runtime_error("Max observer count for scene reached");
    }
#endif

    return nextObserverIndex++;
}

void SceneDescriptorSetInstance::updateObserverCamera(std::uint32_t observerIndex,
                                                      const glm::mat4& cam) {
    cameraBuffer[observerIndex] = cam;
}

} // namespace ds
} // namespace render
} // namespace bl
