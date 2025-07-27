#ifndef BLIB_RENDER_DESCRIPTORS_BUILTIN_COMMONSHADERINPUTS_HPP
#define BLIB_RENDER_DESCRIPTORS_BUILTIN_COMMONSHADERINPUTS_HPP

#include <BLIB/Render/Buffers/StaticUniformBuffer.hpp>
#include <BLIB/Render/Descriptors/BufferShaderInput.hpp>
#include <BLIB/Render/Descriptors/ShaderInput.hpp>
#include <array>
#include <glm/glm.hpp>

namespace bl
{
namespace rc
{
namespace ds
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
using ShadowMapCameraShaderInput =
    BufferShaderInput<buf::StaticUniformBuffer<ShadowMapCameraPayload>,
                      cfg::Limits::MaxSpotShadows + cfg::Limits::MaxPointShadows,
                      buf::Alignment::UboBindOffset>;

/**
 * @brief The name of the shadow map camera input
 *
 * @ingroup Renderer
 */
constexpr const char* ShadowMapCameraInputName = "__builtin__shadow_map_binding";

} // namespace ds
} // namespace rc
} // namespace bl

#endif
