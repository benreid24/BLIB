#ifndef BLIB_ENGINE_ENGINESTATE_HPP
#define BLIB_ENGINE_ENGINESTATE_HPP

#include <memory>

namespace bl
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
class EngineState {
public:
    using Ptr = std::shared_ptr<EngineState>;

    virtual ~EngineState() = default;

    /**
     * @brief This is called each time the state becomes the active engine state
     *
     */
    virtual void makeActive(Engine& engine) = 0;

    /**
     * @brief This is called each time this state is pushed down by a new state
     *
     */
    virtual void onPushedDown(Engine& engine) = 0;

    /**
     * @brief This is called when this state is popped off the state stack entirely
     *
     */
    virtual void onPoppedOff(Engine& engine) = 0;

    /**
     * @brief Perform logic updates
     *
     * @param engine Reference to the main Engine
     * @param dt Elapsed time since last update
     */
    virtual void update(Engine& engine, float dt) = 0;

    /**
     * @brief Render the application to the window owned by the Engine
     *
     * @param engine The main Engine managing the window to render to
     * @param residualLag Residual elapsed time between calls to update(). May be used for
     *                    graphical interpolation
     */
    virtual void render(Engine& engine, float residualLag) = 0;
};

} // namespace bl

#endif
