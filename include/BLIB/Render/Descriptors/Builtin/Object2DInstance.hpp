#ifndef BLIB_RENDER_DESCRIPTORS_BUILTIN_OBJECT2DINSTANCE_HPP
#define BLIB_RENDER_DESCRIPTORS_BUILTIN_OBJECT2DINSTANCE_HPP

#include <BLIB/Render/Descriptors/Builtin/CommonBindings.hpp>

namespace bl
{
namespace rc
{
namespace dsi
{
namespace priv
{
using Object2DBindings = ds::Bindings<Transform2DBinding, TextureBinding>;
} // namespace priv

/**
 * @brief Descriptor set instance used by all 2d objects in the engine default pipelines. Contains
 *        the object transform matrix and texture id
 *
 * @ingroup Renderer
 */
using Object2DInstance = ds::GenericDescriptorSetInstance<priv::Object2DBindings>;

} // namespace dsi
} // namespace rc
} // namespace bl

#endif
