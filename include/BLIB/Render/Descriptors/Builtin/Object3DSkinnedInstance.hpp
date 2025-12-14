#ifndef BLIB_RENDER_DESCRIPTORS_BUILTIN_OBJECT3DSKINNEDINSTANCE_HPP
#define BLIB_RENDER_DESCRIPTORS_BUILTIN_OBJECT3DSKINNEDINSTANCE_HPP

#include <BLIB/Render/Descriptors/Builtin/CommonBindings.hpp>

namespace bl
{
namespace rc
{
namespace dsi
{
namespace priv
{
using Object3DSkinnedBindings = ds::Bindings<Transform3DBinding, MaterialBinding,
                                             SkeletalBonesBinding, SkeletalBonesOffsetBinding>;
}

/**
 * @brief Descriptor set instance used for skinned skeletal meshes
 *
 * @ingroup Renderer
 */
using Object3DSkinnedInstance = ds::GenericDescriptorSetInstance<priv::Object3DSkinnedBindings>;

} // namespace dsi
} // namespace rc
} // namespace bl

#endif
