#ifndef BLIB_RENDER_DESCRIPTORS_BUILTIN_SHADOWMAPFACTORY_HPP
#define BLIB_RENDER_DESCRIPTORS_BUILTIN_SHADOWMAPFACTORY_HPP

#include <BLIB/Render/Descriptors/Builtin/ShadowMapInstance.hpp>
#include <BLIB/Render/Descriptors/GenericDescriptorSetFactory.hpp>

namespace bl
{
namespace rc
{
namespace ds
{
/**
 * @brief Descriptor set factory for the shadow map light cameras
 *
 * @ingroup Renderer
 */
using ShadowMapFactory =
    GenericDescriptorSetFactory<priv::ShadowMapBindings, VK_SHADER_STAGE_VERTEX_BIT>;

} // namespace ds
} // namespace rc
} // namespace bl

#endif
