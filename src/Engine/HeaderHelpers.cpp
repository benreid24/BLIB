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

} // namespace engine
} // namespace bl
