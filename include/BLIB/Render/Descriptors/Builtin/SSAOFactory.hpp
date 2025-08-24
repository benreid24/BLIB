#ifndef BLIB_RENDER_DESCRIPTORS_BUILTIN_SSAOFACTORY_HPP
#define BLIB_RENDER_DESCRIPTORS_BUILTIN_SSAOFACTORY_HPP

#include <BLIB/Render/Descriptors/Builtin/SSAOInstance.hpp>
#include <BLIB/Render/Descriptors/GenericDescriptorSetFactory.hpp>

namespace bl
{
namespace rc
{
namespace dsi
{
/**
 * @brief The factory for the SSAO descriptor set
 *
 * @ingroup Renderer
 */
using SSAOFactory = ds::GenericDescriptorSetFactory<SSAOBindings, VK_SHADER_STAGE_FRAGMENT_BIT>;

} // namespace dsi
} // namespace rc
} // namespace bl

#endif
