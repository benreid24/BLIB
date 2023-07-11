#ifndef BLIB_RENDER_RESOURCES_SHADERMODULECACHE_HPP
#define BLIB_RENDER_RESOURCES_SHADERMODULECACHE_HPP

#include <string>
#include <unordered_map>
#include <BLIB/Vulkan.hpp>

namespace bl
{
namespace gfx
{
namespace vk
{
struct VulkanState;
}

namespace res
{
/**
 * @brief Basic cache for shaders to avoid re-loading/re-compiling the same shaders
 *
 * @ingroup Renderer
 */
class ShaderModuleCache {
public:
    /**
     * @brief Loads a new shader
     *
     * @param path The path to the shader
     * @return The new or existing shader. Nullptr on error
     */
    VkShaderModule loadShader(const std::string& path);

private:
    VkDevice device;
    std::unordered_map<std::string, VkShaderModule> cache;

    ShaderModuleCache() = default;
    void init(VkDevice device);
    void cleanup();

    friend struct vk::VulkanState;
};

} // namespace res
} // namespace gfx
} // namespace bl

#endif
