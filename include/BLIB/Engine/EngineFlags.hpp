#ifndef BLIB_ENGINE_ENGINEFLAGS_HPP
#define BLIB_ENGINE_ENGINEFLAGS_HPP

#include <cstdint>

namespace bl
{
class Enginel;

/**
 * @brief Collection of flags that can be set to modify or control the behavior of the Engine.
 *        All flags are read by the engine at the end of each update loop
 *
 * @ingroup Engine
 *
 */
class EngineFlags {
public:
    /**
     * @brief The flags that may be used to control the Engine
     *
     */
    enum Flag {
        Terminate = 0, /// Terminates the Engine's run() method
        PopState       /// Returns to the previous state. Terminates if no previous state
    };

    /**
     * @brief Sets the given flag to true
     *
     * @param flag The flag to set
     */
    void setFlag(Flag flag);

    /**
     * @brief Returns the state of the given flag
     *
     */
    bool flagSet(Flag flag);

private:
    uint64_t flags;

    EngineFlags();
    void clear();

    friend class Engine;
};

} // namespace bl

#endif
