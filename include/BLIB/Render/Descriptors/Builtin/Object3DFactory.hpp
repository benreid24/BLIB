#ifndef BLIB_RENDER_DESCRIPTORS_BUILTIN_OBJECT3DFACTORY_HPP
#define BLIB_RENDER_DESCRIPTORS_BUILTIN_OBJECT3DFACTORY_HPP

#include <BLIB/Render/Descriptors/Builtin/Object3DInstance.hpp>
#include <BLIB/Render/Descriptors/GenericDescriptorSetFactory.hpp>

namespace bl
{
namespace rc
{
namespace ds
{
/**
 * @brief Descriptor set factory used by meshes rendered using default built-in pipelines
 *
 * @ingroup Renderer
 */
using Object3DFactory =
    GenericDescriptorSetFactory<priv::Object3DBindings, VK_SHADER_STAGE_VERTEX_BIT,
                                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT>;

} // namespace ds
} // namespace rc
} // namespace bl

#endif
