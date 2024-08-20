#ifndef BLIB_GAME_HPP
#define BLIB_GAME_HPP

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Engine/State.hpp>
#include <stdexcept>
#include <type_traits>

int main(int argc, char** argv);

/**
 * @defgroup Entrypoint
 * @brief Contains a class to be implemented by the user
 */

namespace bl
{
/// Contains a class to be implemented by the user
namespace game
{
/**
 * @brief Base class for the main game instance. Apps using the BLIB-provided entrypoint should
 *        create a derived class and instantiate one instance globally before main()
 *
 * #ingroup Game
 */
class Game {
public:
    /**
     * @brief Access the main Game instance
     *
     * @tparam T Optional derived class to cast to
     * @return The main game instance
     */
    template<typename T = Game>
    static T& getInstance() {
        static_assert(std::is_base_of_v<Game, T>, "T must derive from Game");
#ifdef BLIB_DEBUG
        if (!instance) { throw std::runtime_error("Game instance does not exist"); }
#endif

        return static_cast<T&>(*instance);
    }

protected:
    /**
     * @brief Sets this instance to be the main instance
     */
    Game() {
        if (instance) { throw std::runtime_error("Only one instance of Game may exist"); }
        instance = this;
    }

    /**
     * @brief Called at the beginning of main(). Apps should load their configs here
     *
     * @param argc The number of arguments passed to main()
     * @param argv The arguments passed to main()
     * @return True on success, false to exit
     */
    virtual bool performEarlyStartup(int argc, char** argv) = 0;

    /**
     * @brief Derived classes should return the engine creation parameters to use
     */
    virtual bl::engine::Settings createStartupParameters() = 0;

    /**
     * @brief Called once the Engine is created. Apps should configure inputs, register systems, and
     *        perform any other startup tasks here
     *
     * @param engine The game engine instance
     * @return True on success, false to exit
     */
    virtual bool completeStartup(engine::Engine& engine) = 0;

    /**
     * @brief Derived classes should create and return the engine state to begin executing
     */
    virtual bl::engine::State::Ptr createInitialEngineState() = 0;

    /**
     * @brief Called after the main engine loop while the engine instance exists
     */
    virtual void startShutdown() = 0;

    /**
     * @brief Called at the end of main() after the engine instance is destroyed
     */
    virtual void completeShutdown() = 0;

private:
    static Game* instance;

    friend int ::main(int argc, char** argv);
    friend class engine::Engine;
};

} // namespace game
} // namespace bl

#endif
