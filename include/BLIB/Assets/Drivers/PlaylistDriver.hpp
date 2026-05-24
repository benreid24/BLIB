#ifndef BLIB_ASSETS_DRIVERS_PLAYLISTDRIVER_HPP
#define BLIB_ASSETS_DRIVERS_PLAYLISTDRIVER_HPP

#include <BLIB/Assets/Builtin/PlaylistPayload.hpp>
#include <BLIB/Assets/DriverGeneric.hpp>

namespace bl
{
namespace asi
{
/**
 * @brief Driver for playlist assets
 *
 * @ingroup Assets
 */
using PlaylistDriver =
    as::DriverGeneric<PlaylistPayload, util::TemplateString{"Playlist"},
                      as::bdl::AssetBundleConfig{
                          .affinity  = as::bdl::AssetBundleConfig::Affinity::Type,
                          .selection = as::bdl::AssetBundleConfig::Selection::Root,
                          .onMount   = as::bdl::AssetBundleConfig::OnMount::WhenRequested}>;

} // namespace asi
} // namespace bl

#endif