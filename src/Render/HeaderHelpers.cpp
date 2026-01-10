#include <BLIB/Render/HeaderHelpers.hpp>

#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
vk::VulkanLayer& HeaderHelpers::getVulkanLayer(Renderer& renderer) {
    return renderer.vulkanState();
}

} // namespace rc
} // namespace bl
