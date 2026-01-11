#ifndef BLIB_RENDER_SHADERRESOURCES_SHADOWMAPCAMERASHADERRESOURCE_HPP
#define BLIB_RENDER_SHADERRESOURCES_SHADOWMAPCAMERASHADERRESOURCE_HPP

#include <BLIB/Render/Buffers/BufferSingleDeviceLocalSourced.hpp>
#include <BLIB/Render/ShaderResources/BufferShaderResource.hpp>
#include <BLIB/Render/ShaderResources/Key.hpp>
#include <array>
#include <glm/glm.hpp>

namespace bl
{
namespace rc
{
namespace sri
{
/**
 * @brief Uniform payload for shadow map rendering cameras
 *
 * @ingroup Renderer
 */
struct ShadowMapCameraPayload {
    std::array<glm::mat4, 6> viewProj;
    glm::vec4 posAndFarPlane; // xyz = position, w = far plane distance
};

/**
 * @brief Shader input for shadow map cameras
 *
 * @ingroup Renderer
 */
using ShadowMapCameraShaderResource = sr::BufferShaderResource<
    buf::BufferSingleDeviceLocalSourced<ShadowMapCameraPayload, buf::Role::UBOBindSlots>,
    cfg::Limits::MaxSpotShadows + cfg::Limits::MaxPointShadows>;

/**
 * @brief The name of the shadow map camera shader resource
 *
 * @ingroup Renderer
 */
constexpr sr::Key<ShadowMapCameraShaderResource> ShadowMapCameraShaderResourceKey{
    "__builtin__shadow_map_camera_shader_resource"};

} // namespace sri
} // namespace rc
} // namespace bl

#endif
