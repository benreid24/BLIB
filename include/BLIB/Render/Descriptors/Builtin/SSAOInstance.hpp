#ifndef BLIB_RENDER_DESCRIPTORS_BUILTIN_SSAOINSTANCE_HPP
#define BLIB_RENDER_DESCRIPTORS_BUILTIN_SSAOINSTANCE_HPP

#include <BLIB/Render/Descriptors/Generic/BufferBinding.hpp>
#include <BLIB/Render/Descriptors/GenericDescriptorSetInstance.hpp>
#include <BLIB/Render/ShaderResources/SSAOBufferShaderResource.hpp>

namespace bl
{
namespace rc
{
namespace dsi
{
/**
 * @brief The bindings for the SSAO descriptor set
 *
 * @ingroup Renderer
 */
using SSAOBindings =
    ds::Bindings<ds::BufferBinding<sri::SSAOBufferShaderResource, sri::SSAOBufferShaderResourceKey,
                                   sr::StoreKey::Global, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER>>;

/**
 * @brief The descriptor set containing the SSAO parameters
 *
 * @ingroup Renderer
 */
using SSAOInstance = ds::GenericDescriptorSetInstance<SSAOBindings>;

} // namespace dsi
} // namespace rc
} // namespace bl

#endif
