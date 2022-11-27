#ifndef BLIB_ENGINE_FLAGS_HPP
#define BLIB_ENGINE_FLAGS_HPP

#include <cstdint>

namespace bl
{
namespace engine
{
class Engine;
/**
 * @brief Collection of flags that can be set to modify or control the behavior of the Engine.
 *        All flags are read by the engine at the end of each update loop
 *
 * @ingroup Engine
 *
 */
class Flags {
public:
    /**
     * @brief The flags that may be used to control the Engine
     *
     */
    enum Flag : std::uint32_t {
        None      = 0,        /// Special value indicating that no flags are set
        Terminate = 0x1,      /// Terminates the Engine's run() method
        PopState  = 0x1 << 1, /// Returns to the previous state. Terminates if no previous state
        _priv_PushState    = 0x1 << 2, /// Used internally by the engine, do not use directly
        _priv_ReplaceState = 0x1 << 3  /// Used internally by the engine, do not use directly
    };

    /**
     * @brief Sets the given flag to true
     *
     * @param flag The flag to set
     */
    void set(Flag flag);

    /**
     * @brief Returns the state of the given flag
     *
     */
    bool active(Flag flag) const;

    /**
     * @brief Returns whether or not any state change related flag is set. Includes Terminate
     *
     */
    bool stateChangeReady() const;

private:
    uint64_t flags;

    Flags();
    void clear();

    friend class Engine;
};

} // namespace engine
} // namespace bl

#endif
