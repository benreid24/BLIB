#ifndef BLIB_ASSETS_DRIVERS_ANIMATION3DDRIVER_HPP
#define BLIB_ASSETS_DRIVERS_ANIMATION3DDRIVER_HPP

#include <BLIB/Assets/Builtin/Animation3DPayload.hpp>
#include <BLIB/Assets/DriverGeneric.hpp>
#include <BLIB/Models/Animation.hpp>

namespace bl
{
namespace asi
{
/**
 * @brief Driver for 3d model animations
 *
 * @ingroup Assets
 */
using Animation3DDriver =
    as::DriverGeneric<Animation3DPayload, util::TemplateString{"Animation3D"},
                      as::bdl::AssetBundleConfig{
                          .affinity  = as::bdl::AssetBundleConfig::Affinity::Parent,
                          .selection = as::bdl::AssetBundleConfig::Selection::NonRoot,
                          .onMount   = as::bdl::AssetBundleConfig::OnMount::WhenRequested}>;

} // namespace asi
} // namespace bl

#endif
