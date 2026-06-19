#ifndef BLIB_ENGINE_PHASE_HPP
#define BLIB_ENGINE_PHASE_HPP

namespace bl
{
namespace engine
{
/**
 * @brief Represents the various phases the engine can be in
 *
 * @ingroup Engine
 */
enum struct Phase {
    /// The engine is starting up
    Starting,

    /// The engine is running
    Running,

    /// The engine is shutting down
    Stopping
};

} // namespace engine
} // namespace bl

#endif
