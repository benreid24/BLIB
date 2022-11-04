#include <BLIB/Engine/Engine.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Media/Audio.hpp>
#include <BLIB/Resources/GarbageCollector.hpp>
#include <SFML/Window.hpp>
#include <cmath>

namespace bl
{
namespace engine
{
Engine::Engine(const Settings& settings)
: engineSettings(settings)
, renderWindow(nullptr)
, input(*this) {
    entityRegistry.setEventDispatcher(engineEventBus);
    settings.syncToConfig();
    eventBus().subscribe(&input);
}

Engine::~Engine() {
    while (!states.empty()) { states.pop(); }
    newState.reset();
    renderWindow.reset();

#ifndef ON_CI
    audio::AudioSystem::shutdown();
    resource::GarbageCollector::shutdown();
#endif
}

bl::event::Dispatcher& Engine::eventBus() { return engineEventBus; }

entity::Registry& Engine::entities() { return entityRegistry; }

script::Manager& Engine::scriptManager() { return engineScriptManager; }

render::RenderSystem& Engine::renderSystem() { return renderingSystem; }

input::InputSystem& Engine::inputSystem() { return input; }

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

void Engine::popState() { flags().set(Flags::PopState); }

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

    if (engineSettings.createWindow()) {
        if (!reCreateWindow(engineSettings.windowParameters())) { return false; }
    }

    sf::Clock fpsTimer;
    float frameCount = 0.f;

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

                switch (event.type) {
                case sf::Event::Closed:
                    engineEventBus.dispatch<event::Shutdown>({event::Shutdown::WindowClosed});
                    renderWindow->close();
                    return true;

                case sf::Event::LostFocus:
                    engineEventBus.dispatch<event::Paused>({});
                    if (!awaitFocus()) {
                        engineEventBus.dispatch<event::Shutdown>({event::Shutdown::WindowClosed});
                        renderWindow->close();
                        return true;
                    }
                    engineEventBus.dispatch<event::Resumed>({});
                    updateOuterTimer.restart();
                    loopTimer.restart();
                    break;

                case sf::Event::Resized:
                    if (engineSettings.windowParameters().letterBox()) { handleResize(event.size); }
                    break;

                default:
                    break;
                }
            }
        }

        // Update and render
        lag += updateOuterTimer.getElapsedTime().asSeconds();
        updateOuterTimer.restart();
        const float startingLag = lag;
        updateMeasureTimer.restart();
        while (lag >= updateTimestep) {
            const float updateStart = updateMeasureTimer.getElapsedTime().asSeconds();
            renderingSystem.update(updateTimestep);
            states.top()->update(*this, updateTimestep);
            if (engineFlags.active(Flags::PopState) || engineFlags.active(Flags::Terminate) ||
                newState) {
                lag = 0.f;
                break;
            }
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
            const float newTs = std::max(engineSettings.updateTimestep(), updateTimestep * 0.95f);
            BL_LOG_INFO << "Performance improved, adjusting timestep from " << updateTimestep
                        << "s to " << newTs << "s";
            updateTimestep = newTs;
        }
        if (renderWindow) { renderingSystem.cameras().configureView(*renderWindow); }
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

        frameCount += 1.f;
        if (fpsTimer.getElapsedTime().asSeconds() >= 1.f && engineSettings.logFps()) {
            BL_LOG_INFO << "Running at " << frameCount / fpsTimer.getElapsedTime().asSeconds()
                        << " fps";
            frameCount = 0.f;
            fpsTimer.restart();
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

bool Engine::reCreateWindow(const Settings::WindowParameters& params) {
    if (!renderContext) { renderContext = std::make_unique<sf::Context>(); }

    if (renderWindow) { renderWindow->create(params.videoMode(), params.title(), params.style()); }
    else {
        renderWindow =
            std::make_unique<sf::RenderWindow>(params.videoMode(), params.title(), params.style());
    }
    if (!renderWindow->isOpen()) {
        BL_LOG_ERROR << "Failed to create window";
        return false;
    }
    if (!params.icon().empty()) {
        sf::Image icon;
        if (icon.loadFromFile(params.icon())) {
            renderWindow->setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
        }
        else {
            BL_LOG_WARN << "Failed to load icon: " << params.icon();
        }
    }

    engineSettings.withWindowParameters(Settings::WindowParameters(params));
    params.syncToConfig();

    return true;
}

void Engine::handleResize(const sf::Event::SizeEvent& resize) {
    const float ogWidth  = static_cast<float>(engineSettings.windowParameters().videoMode().width);
    const float ogHeight = static_cast<float>(engineSettings.windowParameters().videoMode().height);

    const float newWidth  = static_cast<float>(resize.width);
    const float newHeight = static_cast<float>(resize.height);

    const float xScale = newWidth / ogWidth;
    const float yScale = newHeight / ogHeight;

    // it's ok to change view size here, cameras reset it every frame anyways
    sf::View view(sf::FloatRect(0.f, 0.f, ogWidth, ogHeight));
    sf::FloatRect viewPort(0.f, 0.f, 1.f, 1.f);

    if (xScale >= yScale) { // constrained by height, bars on sides
        viewPort.width = ogWidth * yScale / newWidth;
        viewPort.left  = (1.f - viewPort.width) * 0.5f;
    }
    else { // constrained by width, bars on top and bottom
        viewPort.height = ogHeight * xScale / newHeight;
        viewPort.top    = (1.f - viewPort.height) * 0.5f;
    }

    view.setViewport(viewPort);
    renderWindow->setView(view);
}

} // namespace engine
} // namespace bl
