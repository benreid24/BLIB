#ifndef BLIB_ENGINE_ENGINE_HPP
#define BLIB_ENGINE_ENGINE_HPP

#include <SFML/Graphics/RenderWindow.hpp>
#include <stack>

#include <BLIB/Engine/Events.hpp>
#include <BLIB/Engine/Flags.hpp>
#include <BLIB/Engine/Settings.hpp>
#include <BLIB/Engine/State.hpp>
#include <BLIB/Entities/Registry.hpp>
#include <BLIB/Events/Dispatcher.hpp>
#include <BLIB/Resources.hpp>
#include <BLIB/Scripts/Manager.hpp>

namespace bl
{
/// The core game engine and related classes
namespace engine
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
     * @brief Creates the game engine from the given settings
     *
     * @param settings Settings for the engine to use
     */
    Engine(const Settings& settings);

    /**
     * @brief Returns a reference to the engine wide entity registry
     *
     */
    entity::Registry& entities();

    /**
     * @brief Returns a reference to the primary engine event dispatcher. Engine events and window
     *        events are pushed through this bus
     *
     */
    bl::event::Dispatcher& eventBus();

    /**
     * @brief Returns a reference to the engine's script Manager
     *
     */
    script::Manager& scriptManager();

    /**
     * @brief Returns the settings the engine is using
     *
     */
    const Settings& settings() const;

    /**
     * @brief Returns the flags that can be set to control Engine behavior
     *
     */
    Flags& flags();

    /**
     * @brief Returns a reference to the window the engine has created and is managing. The window
     *        is created when run() is called
     *
     */
    sf::RenderWindow& window();

    /**
     * @brief Runs the main game loop starting in the given initial state. This is the main
     *        application loop and runs for the duration of the program. All setup should be
     *        performed prior to calling this. The sf::RenderWindow is created in here
     *
     * @param initialState The starting engine state
     * @return bool True if the engine exited cleanly, false if exiting due to error
     */
    bool run(State::Ptr initialState);

    /**
     * @brief Sets the next state for the following engine update loop. May be called at any
     *        time, the next state will not be transitioned to until the start of main loop. If
     *        the Popstate flag is set, that will be evaluated before the new state is pushed
     *
     * @param next Next state to enter on the following main loop run
     */
    void nextState(State::Ptr next);

private:
    const Settings engineSettings;
    Flags engineFlags;
    std::stack<State::Ptr> states;
    State::Ptr newState;

    std::shared_ptr<sf::RenderWindow> renderWindow;
    bl::event::Dispatcher engineEventBus;
    script::Manager engineScriptManager;
    entity::Registry entityRegistry;

    bool awaitFocus();
};

} // namespace engine
} // namespace bl

#endif
