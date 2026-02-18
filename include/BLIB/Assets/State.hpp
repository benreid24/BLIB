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
    Known    = 0,  ///< The asset is known to exist by id and type only
    Unloaded = 1,  ///< The asset metadata is available
    Loading  = 2,  ///< The asset payload is being loaded and should not be accessed yet
    Loaded   = 3,  ///< The asset is fully loaded and ready to use
    Failed   = -1, ///< The asset failed to load
    Unknown  = -2  ///< Used when invalid UUIDs are searched
};

} // namespace as
} // namespace bl

/**
 * @brief Comparator for asset states. Higher states represent more complete loading
 *
 * @param lhs The left hand side of the comparison
 * @param rhs The right hand side of the comparison
 * @return True if lhs is less than rhs, false otherwise
 */
inline bool operator<(bl::as::State lhs, bl::as::State rhs) {
    return static_cast<int>(lhs) < static_cast<int>(rhs);
}

#endif
