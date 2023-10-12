#ifndef BLIB_ENGINE_STATEMASK_HPP
#define BLIB_ENGINE_STATEMASK_HPP

#include <cstdint>
#include <type_traits>

namespace bl
{
namespace engine
{
/**
 * @brief Wrapper struct for state masks. An engine state has a mask which determines which systems
 *        run while that state is active. Any systems whose mask overlaps the state mask are
 *        executed each frame while the others are skipped
 *
 * @ingroup Engine
 */
struct StateMask {
    /// The type of the mask
    enum V : std::uint32_t {
        /// Special value to use if no systems should execute
        None = 0x0,

        /// The game is running normally
        Running = 0x1 << 0,

        /// The game is on a pause menu overlaying normal gameplay
        Paused = 0x1 << 1,

        /// The game is in a full screen menu
        Menu = 0x1 << 2,

        /// Special value to use if a system should execute regardless of engine state
        All = 0xFFFFFFFF
    };
};

} // namespace engine
} // namespace bl

/**
 * @brief Explicitly defined binary OR operator for easy typing
 *
 * @param left The left mask
 * @param right The right mask
 * @return The mask containing both left and right
 */
inline bl::engine::StateMask::V operator|(bl::engine::StateMask::V left,
                                          bl::engine::StateMask::V right) {
    using V = bl::engine::StateMask::V;
    using I = std::underlying_type_t<V>;
    return static_cast<V>(static_cast<I>(left) | static_cast<I>(right));
}

#endif
