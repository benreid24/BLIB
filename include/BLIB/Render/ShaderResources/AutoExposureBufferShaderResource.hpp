#ifndef BLIB_RENDER_SHADERRESOURCES_AUTOEXPOSUREBUFFERSHADERRESOURCE_HPP
#define BLIB_RENDER_SHADERRESOURCES_AUTOEXPOSUREBUFFERSHADERRESOURCE_HPP

#include <BLIB/Render/Buffers/StaticSSBO.hpp>
#include <BLIB/Render/ShaderResources/BufferShaderResource.hpp>
#include <BLIB/Render/ShaderResources/Key.hpp>

namespace bl
{
namespace rc
{
namespace sri
{
/**
 * @brief Payload for the auto exposure work buffer used by the compute pipelines
 *
 * @ingroup Renderer
 */
struct AutoExposureBufferPayload {
    float totalLuminosity;
};

/**
 * @brief Shader resource for the work buffer used by the auto exposure compute pipelines
 *
 * @ingroup Renderer
 */
using AutoExposureBufferShaderResource =
    sr::BufferShaderResource<buf::StaticSSBO<AutoExposureBufferPayload>, 1>;

/**
 * @brief Resource key for the work buffer used by the auto exposure compute pipelines
 *
 * @ingroup Renderer
 */
constexpr sr::Key<AutoExposureBufferShaderResource> AutoExposureBufferShaderResourceKey{
    "__builtin_AutoExposureWorkBuffer"};

} // namespace sri
} // namespace rc
} // namespace bl

#endif
