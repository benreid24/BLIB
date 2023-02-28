#ifndef BLIB_ENGINE_STATEMASK_HPP
#define BLIB_ENGINE_STATEMASK_HPP

#include <cstdint>

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
        None = 0,

        /// Special value to use if a system should execute regardless of engine state
        All = 0xFFFFFFFF
    };
};

} // namespace engine
} // namespace bl

#endif
