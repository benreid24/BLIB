#ifndef BLIB_RENDER_DESCRIPTORS_BUILTIN_OBJECT3DINSTANCE_HPP
#define BLIB_RENDER_DESCRIPTORS_BUILTIN_OBJECT3DINSTANCE_HPP

#include <BLIB/Render/Descriptors/Builtin/CommonBindings.hpp>

namespace bl
{
namespace rc
{
namespace ds
{
namespace priv
{
using Object3DBindings = Bindings<Transform3DBinding, MaterialBinding>;
}

/**
 * @brief Descriptor set instance used by all meshes in the engine default pipelines. Contains the
 *        object transform matrix and texture id
 *
 * @ingroup Renderer
 */
using Object3DInstance = GenericDescriptorSetInstance<priv::Object3DBindings>;

} // namespace ds
} // namespace rc
} // namespace bl

#endif
