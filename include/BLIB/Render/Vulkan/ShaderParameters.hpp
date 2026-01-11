#ifndef BLIB_RENDER_VULKAN_SHADERPARAMETERS_HPP
#define BLIB_RENDER_VULKAN_SHADERPARAMETERS_HPP

#include <BLIB/Vulkan.hpp>
#include <string>

namespace bl
{
namespace rc
{
namespace vk
{
/**
 * @brief Parameters for creating a shader module
 *
 * @ingroup Renderer
 */
struct ShaderParameters {
    std::string path;
    VkShaderStageFlags stage;
    std::string entrypoint;

    /**
     * @brief Does nothing
     */
    ShaderParameters() = default;

    /**
     * @brief Sets the shader parameters
     *
     * @param path The path to load the shader from
     * @param stage The stage of the shader module
     * @param entrypoint The main method in the shader
     */
    ShaderParameters(const std::string& path, VkShaderStageFlags stage,
                     const std::string& entrypoint)
    : path(path)
    , stage(stage)
    , entrypoint(entrypoint) {}
};

} // namespace vk
} // namespace rc
} // namespace bl

#endif
