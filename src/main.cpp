#include <BLIB/Game.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Resources/GarbageCollector.hpp>
#include <BLIB/Util/Waiter.hpp>

using namespace bl::game;

Game* Game::instance = nullptr;

int main(int argc, char** argv) {
    if (!Game::instance) {
        BL_LOG_CRITICAL
            << "Game instance does not exist. Please create an instance of bl::game::Game";
        return 1;
    }

    Game& game = Game::getInstance();

    BL_LOG_INFO << "Performing early startup";
    if (!game.performEarlyStartup(argc, argv)) {
        BL_LOG_ERROR << "Game instance early startup failed";
        return 1;
    }

    {
        BL_LOG_INFO << "Creating engine instance";
        bl::engine::Engine engine(game.createStartupParameters());

        BL_LOG_INFO << "Completing startup";
        if (!game.completeStartup(engine)) {
            BL_LOG_ERROR << "Game instance startup completion failed";
            return 1;
        }

        BL_LOG_INFO << "Starting the main engine loop";
        if (!engine.run(game.createInitialEngineState())) {
            BL_LOG_ERROR << "Engine failed to run";
            bl::util::Waiter::unblockAll();
            return 1;
        }

        BL_LOG_INFO << "Beginning shutdown";
        game.startShutdown();

        BL_LOG_INFO << "Unblocking waiting threads";
        bl::util::Waiter::unblockAll();
    }

    BL_LOG_INFO << "Freeing resources";
    bl::resource::GarbageCollector::shutdownAndClear();

    BL_LOG_INFO << "Completing shutdown";
    game.completeShutdown();

    BL_LOG_INFO << "Exiting normally";
    return 0;
}
