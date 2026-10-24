#ifndef BLIB_SYSTEMS_JOLT_CONSTANTS_HPP
#define BLIB_SYSTEMS_JOLT_CONSTANTS_HPP

namespace bl
{
namespace sys
{
namespace jolt
{
/**
 * @brief Jolt physics constants
 *
 * @ingroup Systems
 */
struct Constants {
    static constexpr unsigned int MaxBodies              = 65536;
    static constexpr unsigned int MaxBodyPairs           = 65536;
    static constexpr unsigned int MaxContactConstraints  = 10240;
    static constexpr unsigned int MaxBodyMutexes         = 0; // uses Jolt default
    static constexpr unsigned int CollisionStepsPer60FPS = 1;
};

} // namespace jolt
} // namespace sys
} // namespace bl

#endif
