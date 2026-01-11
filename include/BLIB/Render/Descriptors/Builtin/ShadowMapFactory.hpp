#ifndef BLIB_RENDER_DESCRIPTORS_BUILTIN_SHADOWMAPFACTORY_HPP
#define BLIB_RENDER_DESCRIPTORS_BUILTIN_SHADOWMAPFACTORY_HPP

#include <BLIB/Render/Descriptors/Builtin/ShadowMapInstance.hpp>
#include <BLIB/Render/Descriptors/GenericDescriptorSetFactory.hpp>

namespace bl
{
namespace rc
{
namespace dsi
{
/**
 * @brief Descriptor set factory for the shadow map light cameras
 *
 * @ingroup Renderer
 */
using ShadowMapFactory =
    ds::GenericDescriptorSetFactory<priv::ShadowMapBindings, VK_SHADER_STAGE_VERTEX_BIT |
                                                                 VK_SHADER_STAGE_GEOMETRY_BIT |
                                                                 VK_SHADER_STAGE_FRAGMENT_BIT>;

} // namespace dsi
} // namespace rc
} // namespace bl

#endif
