#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Engine/HeaderHelpers.hpp>

namespace bl
{
namespace engine
{
rc::Renderer& HeaderHelpers::getRenderer(engine::Engine& engine) { return engine.renderer(); }

rc::vk::VulkanState& HeaderHelpers::getVulkanState(engine::Engine& engine) {
    return engine.renderer().vulkanState();
}

ecs::Registry& HeaderHelpers::getRegistry(engine::Engine& engine) { return engine.ecs(); }

sig::Channel& HeaderHelpers::getEngineSignalChannel(engine::Engine& engine) {
    return engine.getSignalChannel();
}

sig::Channel& HeaderHelpers::getRendererSignalChannel(engine::Engine& engine) {
    return engine.renderer().getSignalChannel();
}

} // namespace engine
} // namespace bl
