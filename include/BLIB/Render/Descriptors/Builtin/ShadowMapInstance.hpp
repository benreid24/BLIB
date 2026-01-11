#ifndef BLIB_RENDER_DESCRIPTORS_BUILTIN_SHADOWMAPINFOINSTANCE_HPP
#define BLIB_RENDER_DESCRIPTORS_BUILTIN_SHADOWMAPINFOINSTANCE_HPP

#include <BLIB/Render/Descriptors/Builtin/ShadowMapBinding.hpp>
#include <BLIB/Render/Descriptors/Generic/Bindings.hpp>
#include <BLIB/Render/Descriptors/GenericDescriptorSetInstance.hpp>
#include <glm/glm.hpp>

namespace bl
{
namespace rc
{
namespace dsi
{
namespace priv
{
using ShadowMapBindings = ds::Bindings<ShadowMapBinding>;
}

/**
 * @brief Descriptor set used to render shadow maps. Contains camera matrices for lights
 *
 * @ingroup Renderer
 */
using ShadowMapInstance = ds::GenericDescriptorSetInstance<priv::ShadowMapBindings>;

} // namespace dsi
} // namespace rc
} // namespace bl

#endif
