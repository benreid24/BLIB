#ifndef BLIB_ENGINE_ENGINESTATE_HPP
#define BLIB_ENGINE_ENGINESTATE_HPP

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
    virtual ~EngineState() = default;

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
     * @param dt Time elapsed since last render
     */
    virtual void render(Engine& engine, float dt) = 0;
};

} // namespace bl

#endif
