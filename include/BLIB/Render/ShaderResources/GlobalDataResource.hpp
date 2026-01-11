#ifndef BLIB_RENDER_SHADERRESOURCES_GLOBALDATARESOURCE_HPP
#define BLIB_RENDER_SHADERRESOURCES_GLOBALDATARESOURCE_HPP

#include <BLIB/Render/Buffers/BufferDoubleStaged.hpp>
#include <BLIB/Render/Buffers/BufferSingleDeviceLocalSourced.hpp>
#include <BLIB/Render/Settings.hpp>
#include <BLIB/Render/ShaderResources/BufferShaderResource.hpp>
#include <BLIB/Render/ShaderResources/Key.hpp>

namespace bl
{
namespace rc
{
namespace sri
{
/**
 * @brief Shader payload that contains global renderer settings
 *
 * @ingroup Renderer
 */
struct SettingsUniformPayload {
    Settings::AutoHdrSettings hdrSettings;
    float gamma;
};

/**
 * @brief Shader resource for the global renderer settings
 *
 * @ingroup Renderer
 */
using SettingsUniformResource = sr::BufferShaderResource<
    buf::BufferSingleDeviceLocalSourced<SettingsUniformPayload, buf::Role::UBOBindSlots>, 2>;

/**
 * @brief Resource key for the global renderer settings
 */
constexpr sr::Key<SettingsUniformResource> SettingsUniformResourceKey{"__builtin_SettingsUniform"};

/**
 * @brief Shader payload that contains frame timing information
 *
 * @ingroup Renderer
 */
struct FrameDataUniformPayload {
    float dt;
    float realDt;
    float residual;
    float realResidual;
    float frameDt;
    float realFrameDt;
};

/**
 * @brief Shader resource for the frame timing information
 *
 * @ingroup Renderer
 */
using FrameDataUniformResource =
    sr::BufferShaderResource<buf::BufferDoubleStagedUBO<FrameDataUniformPayload>, 1>;

/**
 * @brief Resource key for the frame timing shader resource
 *
 * @ingroup Renderer
 */
constexpr sr::Key<FrameDataUniformResource> FrameDataUniformResourceKey{
    "__builtin_FrameDataUniform"};

/**
 * @brief Shader payload for dynamic settings that get changed by compute shaders
 *
 * @ingroup Renderer
 */
struct DynamicSettingsUniformPayload {
    float currentHdrExposure;
};

/**
 * @brief Shader resource for the dynamic settings shader payload
 *
 * @ingroup Renderer
 */
using DynamicSettingsUniformResource =
    sr::BufferShaderResource<buf::BufferSingleDeviceLocalSourcedSSBO<DynamicSettingsUniformPayload>,
                             1>;

/**
 * @brief Resource key for the dynamic settings shader resource
 *
 * @ingroup Renderer
 */
constexpr sr::Key<DynamicSettingsUniformResource> DynamicSettingsUniformResourceKey{
    "__builtin_DynamicSettings"};

} // namespace sri
} // namespace rc
} // namespace bl

#endif
