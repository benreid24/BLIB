#ifndef BLIB_ASSETS_STATE_HPP
#define BLIB_ASSETS_STATE_HPP

#include <ostream>

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
    Unloaded = 1,  ///< The asset metadata is available but the payload is not loaded
    Loading  = 2,  ///< The asset payload is being loaded and should not be accessed yet
    Loaded   = 3,  ///< The asset is fully loaded and ready to use
    Failed   = -1, ///< The asset failed to load
    Unknown  = -2, ///< Used when invalid UUIDs are searched
    Missing  = -3  ///< Used when an asset is missing
};

/**
 * @brief Output operator for asset states
 *
 * @param os The output stream
 * @param state The asset state to output
 * @return A reference to the output stream
 */
inline std::ostream& operator<<(std::ostream& os, State state) {
    switch (state) {
    case State::Unloaded:
        return os << "Unloaded";
    case State::Loading:
        return os << "Loading";
    case State::Loaded:
        return os << "Loaded";
    case State::Failed:
        return os << "Failed";
    case State::Unknown:
        return os << "Unknown";
    case State::Missing:
        return os << "Missing";
    default:
        return os << "Invalid(" << static_cast<int>(state) << ")";
    }
}

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
