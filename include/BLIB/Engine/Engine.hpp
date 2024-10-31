#ifndef BLIB_ENGINE_ENGINE_HPP
#define BLIB_ENGINE_ENGINE_HPP

#include <BLIB/ECS/Registry.hpp>
#include <BLIB/Engine/Events.hpp>
#include <BLIB/Engine/Events/Worlds.hpp>
#include <BLIB/Engine/Flags.hpp>
#include <BLIB/Engine/Player.hpp>
#include <BLIB/Engine/Settings.hpp>
#include <BLIB/Engine/State.hpp>
#include <BLIB/Engine/Systems.hpp>
#include <BLIB/Engine/Window.hpp>
#include <BLIB/Engine/Worker.hpp>
#include <BLIB/Engine/World.hpp>
#include <BLIB/Events/Dispatcher.hpp>
#include <BLIB/Input.hpp>
#include <BLIB/Particles/ParticleSystem.hpp>
#include <BLIB/Render/Renderer.hpp>
#include <BLIB/Resources.hpp>
#include <BLIB/Scripts/Manager.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <BLIB/Util/RefPool.hpp>
#include <BLIB/Util/ThreadPool.hpp>
#include <array>
#include <stack>

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
class Engine : private util::NonCopyable {
public:
    /// Type signature of factory methods to create engine states
    using StateFactory = std::function<State::Ptr()>;

    /**
     * @brief Creates the game engine from the given settings
     *
     * @param settings Settings for the engine to use
     */
    Engine(const Settings& settings);

    /**
     * @brief Destroy the Engine object
     */
    ~Engine();

    /**
     * @brief Returns a reference to the engine wide entity registry
     */
    ecs::Registry& ecs();

    /**
     * @brief Returns the systems registry of the engine
     */
    Systems& systems();

    /**
     * @brief Returns a reference to the engine's script Manager
     */
    script::Manager& scriptManager();

    /**
     * @brief Returns the rendering system of the engine
     *
     * @return render::Renderer& The rendering system
     */
    rc::Renderer& renderer();

    /**
     * @brief Returns the user input system of the engine
     *
     * @return input::InputSystem& The user input system
     */
    input::InputSystem& inputSystem();

    /**
     * @brief Returns the engine thread pool
     */
    util::ThreadPool& threadPool();

    /**
     * @brief Returns the threadpool to be used for long running background tasks
     */
    util::ThreadPool& longRunningThreadpool();

    /**
     * @brief Returns the engine particle system
     */
    pcl::ParticleSystem& particleSystem();

    /**
     * @brief Returns the settings the engine is using
     */
    const Settings& settings() const;

    /**
     * @brief Re-creates the game window from the new settings
     *
     * @param parameters The new settings to create the window with
     * @return bool True on success, false on error
     */
    bool reCreateWindow(const Settings::WindowParameters& parameters);

    /**
     * @brief Updates settings on the window without recreating it. Settings that require a new
     *        window are not applied, but do get saved to the configuration store
     *
     * @param parameters The settings to apply
     */
    void updateExistingWindow(const Settings::WindowParameters& parameters);

    /**
     * @brief Returns the flags that can be set to control Engine behavior
     */
    Flags& flags();

    /**
     * @brief Returns a reference to the window the engine has created and is managing. The window
     *        is created when run() is called
     */
    EngineWindow& window();

    /**
     * @brief Runs the main game loop starting in the given initial state. This is the main
     *        application loop and runs for the duration of the program. All setup should be
     *        performed prior to calling this. The EngineWindow is created in here
     *
     * @param initialState The starting engine state
     * @return bool True if the engine exited cleanly, false if exiting due to error
     */
    bool run(State::Ptr initialState);

    /**
     * @brief Same as run but creates the initial state after engine initialization. Useful if the
     *        state constructor relies on the engine being loaded
     *
     * @param stateCreator The factory to create the initial state
     * @return True if the engine exited cleanly, false if exiting due to error
     */
    bool run(StateFactory&& stateCreator);

    /**
     * @brief Sets the next state for the following engine update loop. May be called at any
     *        time, the next state will not be transitioned to until the start of main loop. If
     *        the PopState flag is set, that will be evaluated before the new state is pushed
     *
     * @param next Next state to enter on the following main loop run
     */
    void pushState(State::Ptr next);

    /**
     * @brief Sets the next state for the following engine update loop. May be called at any
     *        time, the next state will not be transitioned to until the start of main loop. This
     *        replaces the current state so that it will not be returned to when the next state is
     *        popped. Equivalent to setting PopState flag and pushing state together
     *
     * @param next Next state to enter on the following main loop run
     */
    void replaceState(State::Ptr next);

    /**
     * @brief Convenience method to set the PopState flag
     */
    void popState();

    /**
     * @brief Sets the scaling factor for simulated time. Essentially multiples real elapsed time by
     *        the given scaling factor. Real elapsed time is multiplied by the factor when the clock
     *        is queried, which may result in additional (or fewer) ticks in order to keep the time
     *        step consistent
     *
     * @param scaleFactor Factor to multiple elapsed time by to determine simulated time
     */
    void setTimeScale(float scaleFactor);

    /**
     * @brief Returns the scaling factor used when determining simulation time from real time
     */
    float getTimeScale() const;

    /**
     * @brief Resets the time scale to 1 so that simulation time matches real time
     */
    void resetTimeScale();

    /**
     * @brief Returns the scale factor being applied to the window to letterbox
     */
    float getWindowScale() const;

    /**
     * @brief Returns the mask for the currently running state
     */
    StateMask::V getCurrentStateMask() const;

    /**
     * @brief Fetches the given player
     *
     * @tparam T The type of player class that is expected
     * @param i The index of the player to fetch
     * @return The player at the given index
     */
    template<typename T = Player>
    T& getPlayer(unsigned int i = 0);

    /**
     * @brief Adds a new player and returns it
     */
    Player& addPlayer();

    /**
     * @brief Adds a new player and returns it
     *
     * @tparam T The derived player class to use
     * @tparam TArgs Argument types to the player constructor
     * @param args Arguments to the player constructor
     */
    template<typename T, typename... TArgs>
    T& addPlayer(TArgs&&... args);

    /**
     * @brief Removes the given player, or the last player if the given index is negative
     *
     * @param i The index of the player to remove or -1 to remove the last player
     */
    void removePlayer(int i = -1);

    /**
     * @brief Creates a new engine World
     *
     * @tparam TWorld The type of world to create
     * @tparam ...TArgs Argument types to the worlds constructor
     * @param ...args Arguments to the worlds constructor
     * @return A ref to the newly created world
     */
    template<typename TWorld, typename... TArgs>
    util::Ref<World, TWorld> createWorld(TArgs&&... args);

    /**
     * @brief Fetch the world at the given index
     *
     * @param index The index of the world to fetch
     * @return The world at the given index
     */
    util::Ref<World> getWorld(unsigned int index);

private:
    Worker worker;
    Settings engineSettings;
    Flags engineFlags;
    std::stack<State::Ptr> states;
    std::vector<std::unique_ptr<Player>> players;
    util::RefPool<World> worldPool;
    std::array<util::Ref<World>, World::MaxWorlds> worlds;
    State::Ptr newState;
    float timeScale;
    float windowScale;

    EngineWindow renderWindow;
    Systems ecsSystems;
    script::Manager engineScriptManager;
    ecs::Registry entityRegistry;
    rc::Renderer renderingSystem;
    input::InputSystem input;
    util::ThreadPool workers;
    util::ThreadPool backgroundWorkers;

    std::optional<std::thread> renderingThread;
    std::mutex renderingMutex;
    std::condition_variable renderingCv;
    std::atomic_bool renderThreadShouldRun;
    void renderThreadBody();

    bool awaitFocus();
    void handleResize(const sf::Event::SizeEvent& resize, bool saveAndSend);
    void postStateChange(State::Ptr& prev);

    bool setup();
    bool loop();
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline ecs::Registry& Engine::ecs() { return entityRegistry; }

inline Systems& Engine::systems() { return ecsSystems; }

inline script::Manager& Engine::scriptManager() { return engineScriptManager; }

inline rc::Renderer& Engine::renderer() { return renderingSystem; }

inline input::InputSystem& Engine::inputSystem() { return input; }

inline const Settings& Engine::settings() const { return engineSettings; }

inline Flags& Engine::flags() { return engineFlags; }

inline EngineWindow& Engine::window() { return renderWindow; }

inline util::ThreadPool& Engine::threadPool() { return workers; }

inline util::ThreadPool& Engine::longRunningThreadpool() { return backgroundWorkers; }

inline float Engine::getWindowScale() const { return windowScale; }

inline StateMask::V Engine::getCurrentStateMask() const {
    return !states.empty() ? states.top()->systemsMask() : StateMask::None;
}

template<typename T>
T& Engine::getPlayer(unsigned int i) {
    if constexpr (std::is_same_v<Player, T>) {
        if (players.empty() && i == 0) {
            BL_LOG_INFO << "No player exists, creating default";
            addPlayer();
        }
    }
    return *players[i];
}

template<typename T, typename... TArgs>
T& Engine::addPlayer(TArgs&&... args) {
    static_assert(std::is_base_of_v<Player, T>, "T must derive from Player");
    static_assert(
        std::is_constructible_v<T, Engine&, rc::Observer*, input::Actor*, TArgs...>,
        "T must be constructible with T(Engine&, rc::Observer*, input::Actor*, TArgs...)");

    auto& observer = renderingSystem.addObserver();
    auto& actor    = input.addActor();
    T* np          = new T(*this, &observer, &actor, args...);
    auto& player   = players.emplace_back(np);
    bl::event::Dispatcher::dispatch<event::PlayerAdded>({*player});
    return *np;
}

template<typename TWorld, typename... TArgs>
util::Ref<World, TWorld> Engine::createWorld(TArgs&&... args) {
    static_assert(std::is_constructible_v<TWorld, Engine&, TArgs...>,
                  "TWorld constructor must take Engine& as first parameter");

    auto ref = worldPool.emplaceDerived<TWorld, TArgs...>(*this, std::forward<TArgs>(args)...);
    for (unsigned int i = 0; i < worlds.size(); ++i) {
        if (!worlds[i].isValid()) {
            worlds[i]  = ref;
            ref->index = i;
            bl::event::Dispatcher::dispatch<event::WorldCreated>({*ref});
            return ref;
        }
    }

    throw std::runtime_error("A maximum of 8 worlds may exist concurrently");
}

inline util::Ref<World> Engine::getWorld(unsigned int index) { return worlds[index]; }

template<typename TWorld, typename... TArgs>
util::Ref<World, TWorld> Player::enterWorld(TArgs&&... args) {
    auto world = owner.createWorld<TWorld>(std::forward<TArgs>(args)...);
    enterWorld(world);
    return world;
}

template<typename TWorld, typename... TArgs>
util::Ref<World, TWorld> Player::changeWorlds(TArgs&&... args) {
    auto world = owner.createWorld<TWorld>(std::forward<TArgs>(args)...);
    changeWorlds(world);
    return world;
}

} // namespace engine
} // namespace bl

#endif
