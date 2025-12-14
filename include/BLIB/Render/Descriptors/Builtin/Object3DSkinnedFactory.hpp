#ifndef BLIB_RENDER_DESCRIPTORS_BUILTIN_OBJECT3DSKINNEDFACTORY_HPP
#define BLIB_RENDER_DESCRIPTORS_BUILTIN_OBJECT3DSKINNEDFACTORY_HPP

#include <BLIB/Render/Descriptors/Builtin/Object3DSkinnedInstance.hpp>
#include <BLIB/Render/Descriptors/GenericDescriptorSetFactory.hpp>

namespace bl
{
namespace rc
{
namespace dsi
{
/**
 * @brief Descriptor set factory used by meshes rendered using default built-in pipelines
 *
 * @ingroup Renderer
 */
using Object3DSkinnedFactory =
    ds::GenericDescriptorSetFactory<priv::Object3DSkinnedBindings, VK_SHADER_STAGE_VERTEX_BIT,
                                    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                                    VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_VERTEX_BIT>;

} // namespace dsi
} // namespace rc
} // namespace bl

#endif
