#include <BLIB/Engine/Engine.hpp>

#include <BLIB/Logging.hpp>
#include <SFML/Window.hpp>
#include <cmath>

namespace bl
{
namespace engine
{
Engine::Engine(const Settings& settings)
: renderWindow(nullptr)
, engineSettings(settings) {}

Engine::~Engine() {
    while (!states.empty()) { states.pop(); }
    newState.reset();
    renderWindow.reset();
}

bl::event::Dispatcher& Engine::eventBus() { return engineEventBus; }

entity::Registry& Engine::entities() { return entityRegistry; }

script::Manager& Engine::scriptManager() { return engineScriptManager; }

const Settings& Engine::settings() const { return engineSettings; }

Flags& Engine::flags() { return engineFlags; }

sf::RenderWindow& Engine::window() { return *renderWindow; }

void Engine::pushState(State::Ptr next) {
    if (newState) {
        BL_LOG_WARN << "pushState called with state " << next->name()
                    << " replaces currently queued next state " << newState->name();
    }
    newState = next;
}

void Engine::replaceState(State::Ptr next) {
    flags().set(Flags::PopState);
    if (newState) {
        BL_LOG_WARN << "replaceState called with state " << next->name()
                    << " replaces currently queued next state " << newState->name();
    }
    newState = next;
}

bool Engine::run(State::Ptr initialState) {
    BL_LOG_INFO << "Starting engine with state: " << initialState->name();
    states.push(initialState);

    sf::Clock loopTimer;
    sf::Clock updateOuterTimer;
    const float minFrameLength =
        engineSettings.maximumFramerate() > 0 ? 1.f / engineSettings.maximumFramerate() : 0.f;
    float lag = 0.f;

    sf::Clock updateMeasureTimer;
    float updateTimestep    = engineSettings.updateTimestep();
    float averageUpdateTime = engineSettings.updateTimestep();

    float lastWarnTime     = -6.f; // always log first warning
    bool followupLog       = false;
    auto fallBehindWarning = [&lastWarnTime, &updateMeasureTimer, &followupLog](float behind) {
        if (updateMeasureTimer.getElapsedTime().asSeconds() - lastWarnTime >= 5.f) {
            lastWarnTime = updateMeasureTimer.getElapsedTime().asSeconds();
            followupLog  = true;
            BL_LOG_WARN << "Can't catch up, running " << behind << " seconds behind";
        }
    };
    auto skipUpdatesInfo = [&followupLog](int frameCount) {
        if (followupLog) {
            followupLog = false;
            BL_LOG_INFO << "Skipping " << frameCount << " updates";
        }
    };

    std::shared_ptr<sf::Context> renderContext; // ensure this thread has active context
    if (engineSettings.createWindow()) {
        renderContext = std::make_shared<sf::Context>();
        renderWindow  = std::make_shared<sf::RenderWindow>(
            engineSettings.videoMode(), engineSettings.windowTitle(), engineSettings.windowStyle());
        if (!renderWindow->isOpen()) {
            BL_LOG_ERROR << "Failed to create window";
            return false;
        }
        if (engineSettings.windowIcon().getSize().x > 0) {
            renderWindow->setIcon(engineSettings.windowIcon().getSize().x,
                                  engineSettings.windowIcon().getSize().y,
                                  engineSettings.windowIcon().getPixelsPtr());
        }
    }

    initialState->activate(*this);
    engineEventBus.dispatch<event::Startup>({initialState});

    while (true) {
        // Clear flags from last loop
        engineFlags.clear();

        // Process window events
        if (renderWindow) {
            sf::Event event;
            while (renderWindow->pollEvent(event)) {
                engineEventBus.dispatch<sf::Event>(event);

                if (event.type == sf::Event::Closed) {
                    engineEventBus.dispatch<event::Shutdown>({event::Shutdown::WindowClosed});
                    renderWindow->close();
                    return true;
                }
                else if (event.type == sf::Event::LostFocus) {
                    engineEventBus.dispatch<event::Paused>({});
                    if (!awaitFocus()) {
                        engineEventBus.dispatch<event::Shutdown>({event::Shutdown::WindowClosed});
                        renderWindow->close();
                        return true;
                    }
                    engineEventBus.dispatch<event::Resumed>({});
                    updateOuterTimer.restart();
                    loopTimer.restart();
                }
                // more events?
            }
        }

        // Update and render
        lag += updateOuterTimer.getElapsedTime().asSeconds();
        updateOuterTimer.restart();
        const float startingLag = lag;
        updateMeasureTimer.restart();
        while (lag >= updateTimestep) {
            const float updateStart = updateMeasureTimer.getElapsedTime().asSeconds();
            states.top()->update(*this, updateTimestep);
            lag -= updateTimestep;
            averageUpdateTime =
                0.8f * averageUpdateTime +
                0.2f * (updateMeasureTimer.getElapsedTime().asSeconds() - updateStart);
            if (updateMeasureTimer.getElapsedTime().asSeconds() > startingLag * 1.1f) {
                fallBehindWarning(updateMeasureTimer.getElapsedTime().asSeconds() - startingLag);
                if (engineSettings.allowVariableTimestep()) {
                    const float newTs = updateTimestep * 1.05f;
                    BL_LOG_INFO << "Adjusting update timestep from " << updateTimestep << "s to "
                                << newTs << "s";
                    updateTimestep    = newTs;
                    averageUpdateTime = updateTimestep;
                }
                else {
                    skipUpdatesInfo(std::ceil(lag / updateTimestep));
                    lag = 0.f;
                }
            }
        }
        if (averageUpdateTime < updateTimestep * 0.9f &&
            updateTimestep > engineSettings.updateTimestep()) {
            float newTs = (1 - averageUpdateTime / updateTimestep) / 2.f * updateTimestep;
            if (newTs < engineSettings.updateTimestep()) newTs = engineSettings.updateTimestep();
            BL_LOG_INFO << "Performance improved, adjusting timestep from " << updateTimestep
                        << "s to " << newTs << "s";
            updateTimestep = newTs;
        }
        states.top()->render(*this, lag);

        // Process flags
        if (engineFlags.active(Flags::Terminate)) {
            engineEventBus.dispatch<event::Shutdown>({event::Shutdown::Terminated});
            if (renderWindow) renderWindow->close();
            return true;
        }
        else if (engineFlags.active(Flags::PopState)) {
            BL_LOG_INFO << "Popping state: " << states.top()->name();
            auto prev = states.top();
            prev->deactivate(*this);
            states.pop();
            if (states.empty() && !newState) { // exit if no states left
                BL_LOG_INFO << "Final state popped, exiting";
                engineEventBus.dispatch<event::Shutdown>({event::Shutdown::FinalStatePopped});
                if (renderWindow) renderWindow->close();
                return true;
            }
            else if (!newState) { // plain pop state
                BL_LOG_INFO << "New engine state (popped): " << states.top()->name();
                states.top()->activate(*this);
                engineEventBus.dispatch<event::StateChange>({states.top(), prev});
            }
            else { // replace state
                BL_LOG_INFO << "New engine state (replaced): " << newState->name();
                states.push(newState);
                states.top()->activate(*this);
                engineEventBus.dispatch<event::StateChange>({states.top(), prev});
                newState = nullptr;
            }
            updateOuterTimer.restart();
            loopTimer.restart();
        }

        // Handle state push
        if (newState) {
            BL_LOG_INFO << "New engine state (pushed): " << newState->name();
            auto prev = states.top();
            prev->deactivate(*this);
            states.push(newState);
            states.top()->activate(*this);
            engineEventBus.dispatch<event::StateChange>({states.top(), prev});
            newState = nullptr;
            updateOuterTimer.restart();
            loopTimer.restart();
        }

        // Adhere to FPS cap
        if (minFrameLength > 0) {
            const float st = minFrameLength - loopTimer.getElapsedTime().asSeconds();
            if (st > 0) sf::sleep(sf::seconds(st));
            loopTimer.restart();
        }
    }

    return false; // shouldn't be able to get here
}

bool Engine::awaitFocus() {
    sf::Event event;
    while (renderWindow->waitEvent(event)) {
        if (event.type == sf::Event::Closed) return false;
        if (event.type == sf::Event::GainedFocus) return true;
    }
    return false;
}

} // namespace engine
} // namespace bl
