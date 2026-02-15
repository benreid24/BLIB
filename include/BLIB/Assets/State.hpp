#ifndef BLIB_ASSETS_STATE_HPP
#define BLIB_ASSETS_STATE_HPP

namespace bl
{
namespace as
{
/**
 * @brief Represents the state of an asset
 *
 * @ingroup Assets
 */
enum struct State {
    Known,    ///< The asset is known to exist by id and type only
    Unloaded, ///< The asset metadata is available
    Loading,  ///< The asset payload is being loaded and should not be accessed yet
    Loaded,   ///< The asset is fully loaded and ready to use
    Failed,   ///< The asset failed to load
    Unknown   ///< Used when invalid UUIDs are searched
};

} // namespace as
} // namespace bl

#endif
