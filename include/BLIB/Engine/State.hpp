#ifndef BLIB_ENGINE_STATE_HPP
#define BLIB_ENGINE_STATE_HPP

#include <BLIB/Engine/StateMask.hpp>
#include <BLIB/Logging.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <memory>
#include <stdexcept>

namespace bl
{
namespace engine
{
class Engine;
/**
 * @brief Base interface for all engine states. The Engine class manages the main game loop,
 *        application logic should be implemented in a set of EngineState classes which can be
 *        plugged into the Engine
 *
 * @ingroup Engine
 *
 */
class State : private util::NonCopyable {
public:
    using Ptr = std::shared_ptr<State>;

    /**
     * @brief Creates a new engine state
     *
     * @param mask Mask to use when selecting which systems to run
     */
    State(StateMask::V mask);

    /**
     * @brief Destroys the state
     */
    virtual ~State() = default;

    /**
     * @brief Returns the mask to use when selecting which systems to run
     */
    constexpr StateMask::V systemsMask() const;

    /**
     * @brief A human readable name of the state for logging
     *
     */
    virtual const char* name() const = 0;

    /**
     * @brief This is called each time the state becomes the active engine state
     *
     */
    virtual void activate(Engine& engine) = 0;

    /**
     * @brief This is called each time this state is deactivated
     *
     */
    virtual void deactivate(Engine& engine) = 0;

    /**
     * @brief Perform logic updates. This is called before any systems are invoked once per tick
     *
     * @param engine Reference to the main Engine
     * @param dt Elapsed simulation time since last update in seconds
     * @param realDt Elapsed time in real time since last update
     */
    virtual void update(Engine& engine, float dt, float realDt) = 0;

    /**
     * @brief Render the application to the window owned by the Engine. Deprecated. Use Renderer
     *
     * @param engine The main Engine managing the window to render to
     * @param residualLag Residual elapsed time between calls to update(). May be used for
     *                    graphical interpolation
     */
    virtual void render(Engine& engine, float residualLag) = 0;

private:
    const StateMask::V mask;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline State::State(StateMask::V m)
: mask(m) {
#ifdef BLIB_DEBUG
    if (m == 0) {
        BL_LOG_CRITICAL << "StateMask of 0 is not supported";
        throw std::runtime_error("StateMask of 0 is not supported");
    }
#endif
}

inline constexpr StateMask::V State::systemsMask() const { return mask; }

} // namespace engine
} // namespace bl

#endif
