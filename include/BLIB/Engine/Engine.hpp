#ifndef BLIB_ENGINE_ENGINE_HPP
#define BLIB_ENGINE_ENGINE_HPP

#include <SFML/Graphics/RenderWindow.hpp>

#include <BLIB/Engine/EngineEvent.hpp>
#include <BLIB/Engine/EngineState.hpp>
#include <BLIB/Util/EventDispatcher.hpp>

namespace bl
{
/**
 * @brief Core engine class that owns engine resources and the main game loop
 *
 * @ingroup Engine
 *
 */
class Engine {
public:
    /**
     * @brief Creates the game engine with the given window and target frame rates
     *
     * @param window The window to manage and render to
     * @param targetRenderFps How many times to render per second
     * @param targetUpdateFps How many logic updates to process per second
     */
    Engine(sf::RenderWindow& window, float targetRenderFps = 60, float targetUpdateFps = 120);

    /**
     * @brief Returns a reference to the engine event dispatcher
     *
     */
    EngineEventDispatcher& engineEventDispatcher();

    /**
     * @brief Returns a reference to the window event dispatcher
     *
     */
    WindowEventDispatcher& windowEventDispatcher();

    /**
     * @brief Returns a reference to the window being managed
     *
     */
    sf::RenderWindow& window();

    /**
     * @brief Runs the main game loop starting in the given initial state. This is the main
     *        application loop and runs for the duration of the program. All setup should be
     *        performed prior to calling this
     *
     * @param initialState The starting engine state
     * @return int Return code of the program
     */
    int run(EngineState& initialState);

private:
    const float targetRenderFps;
    const float targetUpdateFps;
    sf::RenderWindow& renderWindow;
    EngineEventDispatcher engineEventBus;
    WindowEventDispatcher windowEventBus;
};

} // namespace bl

#endif
