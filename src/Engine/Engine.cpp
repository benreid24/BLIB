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
, entityRegistry(settings.maximumEntityCount())
, input(*this) {
    settings.syncToConfig();
    bl::event::Dispatcher::subscribe(&input);
}

Engine::~Engine() {
    bl::event::Dispatcher::clearAllListeners();
    while (!states.empty()) { states.pop(); }
    newState.reset();

#ifndef ON_CI
    audio::AudioSystem::shutdown();
#endif
}

ecs::Registry& Engine::ecs() { return entityRegistry; }

script::Manager& Engine::scriptManager() { return engineScriptManager; }

render::Renderer& Engine::renderer() { return renderingSystem; }

input::InputSystem& Engine::inputSystem() { return input; }

const Settings& Engine::settings() const { return engineSettings; }

Flags& Engine::flags() { return engineFlags; }

sf::RenderWindow& Engine::window() { return renderWindow; }

void Engine::pushState(State::Ptr next) {
    flags().set(Flags::_priv_PushState);
    if (newState) {
        BL_LOG_WARN << "pushState called with state " << next->name()
                    << " replaces currently queued next state " << newState->name();
    }
    newState = next;
}

void Engine::replaceState(State::Ptr next) {
    flags().set(Flags::_priv_ReplaceState);
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
    bl::event::Dispatcher::dispatch<event::Startup>({initialState});

    while (true) {
        // Clear flags from last loop
        engineFlags.clear();

#ifndef ON_CI // Process window events
        sf::Event event;
        while (renderWindow.pollEvent(event)) {
            bl::event::Dispatcher::dispatch<sf::Event>(event);

            switch (event.type) {
            case sf::Event::Closed:
                bl::event::Dispatcher::dispatch<event::Shutdown>({event::Shutdown::WindowClosed});
                renderWindow.close();
                return true;

            case sf::Event::LostFocus:
                bl::event::Dispatcher::dispatch<event::Paused>({});
                if (!awaitFocus()) {
                    bl::event::Dispatcher::dispatch<event::Shutdown>(
                        {event::Shutdown::WindowClosed});
                    renderWindow.close();
                    return true;
                }
                bl::event::Dispatcher::dispatch<event::Resumed>({});
                updateOuterTimer.restart();
                loopTimer.restart();
                break;

            case sf::Event::Resized:
                if (engineSettings.windowParameters().letterBox()) {
                    handleResize(event.size, true);
                }
                break;

            default:
                break;
            }
        }
#endif

        // Update and render
        lag += updateOuterTimer.getElapsedTime().asSeconds();
        updateOuterTimer.restart();
        const float startingLag = lag;
        updateMeasureTimer.restart();

        // update until caught up
        while (lag >= updateTimestep) {
            const float updateStart = updateMeasureTimer.getElapsedTime().asSeconds();

            // core update game logic
            renderingSystem.update(updateTimestep);
            input.update();
            states.top()->update(*this, updateTimestep);
            bl::event::Dispatcher::syncListeners();

            // check if we should end early
            if (engineFlags.stateChangeReady()) {
                lag = 0.f;
                break;
            }

            // handle timing
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

        // update timing
        if (averageUpdateTime < updateTimestep * 0.9f &&
            updateTimestep > engineSettings.updateTimestep()) {
            const float newTs = std::max(engineSettings.updateTimestep(), updateTimestep * 0.95f);
            BL_LOG_INFO << "Performance improved, adjusting timestep from " << updateTimestep
                        << "s to " << newTs << "s";
            updateTimestep = newTs;
        }

#ifndef ON_CI // do render
        if (!engineFlags.active(Flags::PopState) && !engineFlags.active(Flags::Terminate) &&
            !newState) {
            renderingSystem.cameras().configureView(renderWindow);
            states.top()->render(*this, lag);
        }
#endif

        // Process flags
        while (engineFlags.stateChangeReady()) {
            if (engineFlags.active(Flags::Terminate)) {
                bl::event::Dispatcher::dispatch<event::Shutdown>({event::Shutdown::Terminated});
#ifndef ON_CI
                renderWindow.close();
#endif
                return true;
            }
            else if (engineFlags.active(Flags::PopState)) {
                BL_LOG_INFO << "Popping state: " << states.top()->name();
                auto prev = states.top();
                prev->deactivate(*this);
                states.pop();
                if (states.empty()) { // exit if no states left
                    BL_LOG_INFO << "Final state popped, exiting";
                    bl::event::Dispatcher::dispatch<event::Shutdown>(
                        {event::Shutdown::FinalStatePopped});
#ifndef ON_CI
                    renderWindow.close();
#endif
                    return true;
                }
                BL_LOG_INFO << "New engine state (popped): " << states.top()->name();
                engineFlags.clear();
                states.top()->activate(*this);
                bl::event::Dispatcher::dispatch<event::StateChange>({states.top(), prev});
            }
            else if (engineFlags.active(Flags::_priv_ReplaceState)) {
                BL_LOG_INFO << "New engine state (replaced): " << newState->name();
                auto prev = states.top();
                prev->deactivate(*this);
                states.pop();
                states.push(newState);
                engineFlags.clear();
                newState.reset();
                states.top()->activate(*this);
                bl::event::Dispatcher::dispatch<event::StateChange>({states.top(), prev});
            }
            else if (engineFlags.active(Flags::_priv_PushState)) {
                BL_LOG_INFO << "New engine state (pushed): " << newState->name();
                auto prev = states.top();
                prev->deactivate(*this);
                states.push(newState);
                engineFlags.clear();
                newState.reset();
                states.top()->activate(*this);
                bl::event::Dispatcher::dispatch<event::StateChange>({states.top(), prev});
            }

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
    while (renderWindow.waitEvent(event)) {
        if (event.type == sf::Event::Closed) return false;
        if (event.type == sf::Event::GainedFocus) return true;
    }
    return false;
}

bool Engine::reCreateWindow(const Settings::WindowParameters& params) {
    renderWindow.create(params.videoMode(), params.title(), params.style());
    if (!renderWindow.isOpen()) {
        BL_LOG_ERROR << "Failed to create window";
        return false;
    }
    if (!params.icon().empty()) {
        sf::Image icon;
        if (resource::ResourceManager<sf::Image>::initializeExisting(params.icon(), icon)) {
            renderWindow.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
        }
        else { BL_LOG_WARN << "Failed to load icon: " << params.icon(); }
    }

    // also saves to config
    updateExistingWindow(params);

    return true;
}

void Engine::updateExistingWindow(const Settings::WindowParameters& params) {
    renderWindow.setTitle(params.title());
    renderWindow.setVerticalSyncEnabled(params.vsyncEnabled());

    engineSettings.withWindowParameters(params);
    params.syncToConfig();

    if (params.initialViewSize().x > 0.f) {
        sf::View view = renderWindow.getView();
        view.setSize(params.initialViewSize());
        view.setCenter(params.initialViewSize() * 0.5f);
        renderWindow.setView(view);
    }

    if (params.letterBox()) {
        sf::Event::SizeEvent e;
        e.width  = renderWindow.getSize().x;
        e.height = renderWindow.getSize().y;
        handleResize(e, false);
    }
}

void Engine::handleResize(const sf::Event::SizeEvent& resize, bool ss) {
    const sf::Vector2f modeSize(sf::Vector2u(engineSettings.windowParameters().videoMode().width,
                                             engineSettings.windowParameters().videoMode().height));
    const sf::Vector2f& ogSize = engineSettings.windowParameters().initialViewSize().x > 0.f ?
                                     engineSettings.windowParameters().initialViewSize() :
                                     modeSize;

    const float newWidth  = static_cast<float>(resize.width);
    const float newHeight = static_cast<float>(resize.height);

    const float xScale = newWidth / ogSize.x;
    const float yScale = newHeight / ogSize.y;

    // it's ok to change view size here, cameras reset it every frame anyways
    sf::View view(sf::FloatRect(0.f, 0.f, ogSize.x, ogSize.y));
    sf::FloatRect viewPort(0.f, 0.f, 1.f, 1.f);

    if (xScale >= yScale) { // constrained by height, bars on sides
        viewPort.width = ogSize.x * yScale / newWidth;
        viewPort.left  = (1.f - viewPort.width) * 0.5f;
    }
    else { // constrained by width, bars on top and bottom
        viewPort.height = ogSize.y * xScale / newHeight;
        viewPort.top    = (1.f - viewPort.height) * 0.5f;
    }

    view.setViewport(viewPort);
    renderWindow.setView(view);

    if (ss) {
        Settings::WindowParameters params = engineSettings.windowParameters();
        params.withVideoMode(
            sf::VideoMode(resize.width, resize.height, params.videoMode().bitsPerPixel));
        engineSettings.withWindowParameters(params);
        params.syncToConfig();
        bl::event::Dispatcher::dispatch<event::WindowResized>({renderWindow});
    }
}

} // namespace engine
} // namespace bl
