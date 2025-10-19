#ifndef BLIB_RENDER_DESCRIPTORS_BUILTIN_AUTOEXPOSUREWORKBUFFER_HPP
#define BLIB_RENDER_DESCRIPTORS_BUILTIN_AUTOEXPOSUREWORKBUFFER_HPP

#include <BLIB/Render/Descriptors/Generic/BufferBinding.hpp>
#include <BLIB/Render/Descriptors/GenericDescriptorSetFactory.hpp>
#include <BLIB/Render/Descriptors/GenericDescriptorSetInstance.hpp>
#include <BLIB/Render/ShaderResources/AutoExposureBufferShaderResource.hpp>

namespace bl
{
namespace rc
{
namespace dsi
{
/**
 * @brief Binding for the auto exposure compute buffer
 *
 * @ingroup Renderer
 */
using AutoExposureBufferBinding =
    ds::BufferBinding<sri::AutoExposureBufferShaderResource,
                      sri::AutoExposureBufferShaderResourceKey, sr::StoreKey::Observer>;

/**
 * @brief Bindings set for the auto exposure compute pipeline layout
 *
 * @ingroup Renderer
 */
using AutoExposureBufferBindings = ds::Bindings<AutoExposureBufferBinding>;

/**
 * @brief Descriptor set instance for the auto exposure compute pipeline layout
 *
 * @ingroup Renderer
 */
using AutoExposureBufferInstance = ds::GenericDescriptorSetInstance<AutoExposureBufferBindings>;

/**
 * @brief Descriptor set factory for the auto exposure compute pipeline layout
 *
 * @ingroup Renderer
 */
using AutoExposureBufferFactory =
    ds::GenericDescriptorSetFactory<AutoExposureBufferBindings, VK_SHADER_STAGE_COMPUTE_BIT>;

} // namespace dsi
} // namespace rc
} // namespace bl

#endif
