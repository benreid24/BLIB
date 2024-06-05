#ifndef BLIB_RENDER_DESCRIPTORS_BUILTIN_OBJECT2DFACTORY_HPP
#define BLIB_RENDER_DESCRIPTORS_BUILTIN_OBJECT2DFACTORY_HPP

#include <BLIB/Render/Descriptors/Builtin/Object2DInstance.hpp>
#include <BLIB/Render/Descriptors/GenericDescriptorSetFactory.hpp>

namespace bl
{
namespace rc
{
namespace ds
{
/**
 * @brief Descriptor set factory used by 2d objects rendered using default built-in pipelines
 *
 * @ingroup Renderer
 */
using Object2DFactory =
    GenericDescriptorSetFactory<priv::Object2DBindings, VK_SHADER_STAGE_VERTEX_BIT,
                                VK_SHADER_STAGE_VERTEX_BIT>;

} // namespace ds
} // namespace rc
} // namespace bl

#endif
