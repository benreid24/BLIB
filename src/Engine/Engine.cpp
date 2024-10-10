#include <BLIB/Engine/Engine.hpp>

#include <BLIB/Audio.hpp>
#include <BLIB/Cameras/OverlayCamera.hpp>
#include <BLIB/Logging.hpp>
#include <BLIB/Particles/ParticleSystem.hpp>
#include <BLIB/Resources/GarbageCollector.hpp>
#include <BLIB/Resources/State.hpp>
#include <BLIB/Systems.hpp>
#include <BLIB/Systems/MarkedForDeath.hpp>
#include <SFML/Window.hpp>
#include <cmath>

namespace bl
{
namespace engine
{
Engine::Engine(const Settings& settings)
: engineSettings(settings)
, timeScale(1.f)
, windowScale(1.f)
, ecsSystems(*this)
, entityRegistry()
, renderingSystem(*this, renderWindow)
, renderThreadShouldRun(true)
, input(*this) {
    settings.syncToConfig();

    addPlayer();

    systems().registerSystem<sys::TogglerSystem>(FrameStage::Update0, StateMask::All);
    systems().registerSystem<sys::VelocitySystem>(FrameStage::Animate,
                                                  StateMask::Running | engine::StateMask::Editor);
    systems().registerSystem<pcl::ParticleSystem>(FrameStage::Update0, StateMask::All);
    systems().registerSystem<sys::MarkedForDeath>(FrameStage::Update0, StateMask::All);
    systems().registerSystem<sys::Physics2D>(FrameStage::Physics, StateMask::Running);

    bl::event::Dispatcher::subscribe(&input);
}

Engine::~Engine() {
    resource::State::appExiting = true;

    if (renderingThread.has_value()) {
        renderThreadShouldRun = false;
        renderingCv.notify_one();
        renderingThread.value().join();
    }

    backgroundWorkers.shutdown();
    workers.shutdown();
    bl::event::Dispatcher::clearAllListeners();
    if (renderingSystem.vulkanState().device) {
        vkCheck(vkDeviceWaitIdle(renderingSystem.vulkanState().device));
    }

    while (!states.empty()) {
        if (states.top().use_count() != 1) {
            BL_LOG_ERROR << "Dangling pointer to state: " << states.top()->name();
        }
        states.pop();
    }
    if (newState && newState.use_count() != 1) {
        BL_LOG_ERROR << "Dangling pointer to state: " << newState->name();
    }
    newState.reset();

    players.clear();
    for (auto& worldRef : worlds) {
        if (worldRef.isValid()) {
            if (worldRef.refCount() > 1) {
                BL_LOG_WARN << "Dangling reference(s) to world " << worldRef->worldIndex();
            }
            worldRef.release();
        }
    }
    worldPool.clear();

    if (renderWindow.isOpen()) { renderWindow.close(); }

    systems().earlyCleanup();
    entityRegistry.destroyAllEntities();

    audio::AudioSystem::shutdown();
    resource::GarbageCollector::get().clear();
    systems().cleanup();

    if (renderWindow.isOpen()) {
        renderingSystem.cleanup();
        renderWindow.close();
    }

    // reset resource manager state for other instances
    resource::State::appExiting = false;
}

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
    if (!setup()) { return false; }
    states.push(std::move(initialState));
    return loop();
}

bool Engine::run(StateFactory&& factory) {
    if (!setup()) { return false; }
    states.push(factory());
    return loop();
}

bool Engine::setup() {
    if (engineSettings.createWindow()) {
        if (!reCreateWindow(engineSettings.windowParameters())) { return false; }
        renderingSystem.initialize();
        if (engineSettings.windowParameters().letterBox()) {
            sf::Event::SizeEvent e{};
            e.width  = renderWindow.getSfWindow().getSize().x;
            e.height = renderWindow.getSfWindow().getSize().y;
            handleResize(e, false);
        }

        renderingThread.emplace(&Engine::renderThreadBody, this);
    }
    ecsSystems.init();
    return true;
}

bool Engine::loop() {
    BL_LOG_INFO << "Starting engine with state: " << states.top()->name();

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

    sf::Clock fpsTimer;
    float frameCount = 0.f;

    workers.start();
    backgroundWorkers.start(2);
    states.top()->activate(*this);
    bl::event::Dispatcher::dispatch<event::Startup>({states.top()});

    while (true) {
        // Clear flags from last loop
        engineFlags.clear();

        if (renderWindow.isOpen()) {
            sf::Event event;
            while (renderWindow.pollEvent(event)) {
                bl::event::Dispatcher::dispatch<sf::Event>(event);

                switch (event.type) {
                case sf::Event::Closed:
                    bl::event::Dispatcher::dispatch<event::Shutdown>(
                        {event::Shutdown::WindowClosed});
                    return true;

                case sf::Event::LostFocus:
                    bl::event::Dispatcher::dispatch<event::Paused>({});
                    if (!awaitFocus()) {
                        bl::event::Dispatcher::dispatch<event::Shutdown>(
                            {event::Shutdown::WindowClosed});
                        return true;
                    }
                    bl::event::Dispatcher::dispatch<event::Resumed>({});
                    updateOuterTimer.restart();
                    loopTimer.restart();
                    break;

                case sf::Event::Resized:
                    handleResize(event.size, true);
                    break;

                default:
                    break;
                }
            }
        }

        ecsSystems.notifyFrameStart();

        // Update and render
        lag += updateOuterTimer.getElapsedTime().asSeconds() * timeScale;
        updateOuterTimer.restart();
        const float startingLag = lag;
        float totalDt           = 0.f;
        updateMeasureTimer.restart();

        // update until caught up
        while (lag >= updateTimestep) {
            const float updateStart = updateMeasureTimer.getElapsedTime().asSeconds();

            // core update game logic
            totalDt += updateTimestep;
            input.update();
            states.top()->update(*this, updateTimestep, updateTimestep / timeScale);
            ecsSystems.update(FrameStage::FrameStart,
                              FrameStage::MARKER_OncePerFrame,
                              states.top()->systemsMask(),
                              updateTimestep,
                              updateTimestep / timeScale,
                              lag,
                              lag / timeScale);
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

        // wait for render thread to finish current frame & block it
        std::unique_lock renderLock(renderingMutex);

        // Free world slots
        for (auto& world : worlds) {
            if (world && world.refCount() == 1) {
                bl::event::Dispatcher::dispatch<event::WorldDestroyed>({*world});
                world.release();
            }
        }

        // Process flags
        bool stateChanged = false;
        while (engineFlags.stateChangeReady()) {
            stateChanged = true;

            if (engineFlags.active(Flags::Terminate)) {
                bl::event::Dispatcher::dispatch<event::Shutdown>({event::Shutdown::Terminated});
                return true;
            }
            else if (engineFlags.active(Flags::PopState)) {
                BL_LOG_INFO << "Popping state: " << states.top()->name();
                State::Ptr prev = states.top();
                prev->deactivate(*this);
                states.pop();
                if (states.empty()) { // exit if no states left
                    BL_LOG_INFO << "Final state popped, exiting";
                    bl::event::Dispatcher::dispatch<event::Shutdown>(
                        {event::Shutdown::FinalStatePopped});
                    return true;
                }
                BL_LOG_INFO << "New engine state (popped): " << states.top()->name();
                postStateChange(prev);
            }
            else if (engineFlags.active(Flags::_priv_ReplaceState)) {
                BL_LOG_INFO << "New engine state (replaced): " << newState->name();
                State::Ptr prev = states.top();
                prev->deactivate(*this);
                states.pop();
                states.push(newState);
                postStateChange(prev);
            }
            else if (engineFlags.active(Flags::_priv_PushState)) {
                BL_LOG_INFO << "New engine state (pushed): " << newState->name();
                State::Ptr& prev = states.top();
                prev->deactivate(*this);
                states.push(newState);
                postStateChange(prev);
            }

            updateOuterTimer.restart();
            loopTimer.restart();
        }

        if (!stateChanged) {
            // signal render next frame
            if (renderWindow.isOpen()) {
                // sync descriptors
                ecsSystems.update(FrameStage::MARKER_OncePerFrame,
                                  FrameStage::COUNT,
                                  states.top()->systemsMask(),
                                  totalDt,
                                  totalDt / timeScale,
                                  lag,
                                  lag / timeScale);

                // flush scene object changes
                renderingSystem.syncSceneObjects();

                // Flush ECS deletion queues
                entityRegistry.flushDeletions();

                // signal rendering thread to start
                renderLock.unlock();
                renderingCv.notify_one();
            }

            // Adhere to FPS cap
            if (minFrameLength > 0) {
                const float st = minFrameLength - loopTimer.getElapsedTime().asSeconds();
                if (st > 0) sf::sleep(sf::seconds(st));
                loopTimer.restart();
            }

            frameCount += 1.f;
            if (renderWindow.isOpen() && fpsTimer.getElapsedTime().asSeconds() >= 1.f &&
                engineSettings.logFps()) {
                const float fps = frameCount / fpsTimer.getElapsedTime().asSeconds();
                renderWindow.getSfWindow().setTitle(engineSettings.windowParameters().title() +
                                                    " (" + std::to_string(int(std::roundf(fps))) +
                                                    " fps)");
                frameCount = 0.f;
                fpsTimer.restart();
            }
        }
        else {
            // Flush ECS deletion queues
            entityRegistry.flushDeletions();
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
            renderWindow.getSfWindow().setIcon(
                icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
        }
        else { BL_LOG_WARN << "Failed to load icon: " << params.icon(); }
    }

    if (renderingSystem.vulkanState().device) { renderingSystem.processWindowRecreate(); }

    // also saves to config
    updateExistingWindow(params);

    return true;
}

void Engine::updateExistingWindow(const Settings::WindowParameters& params) {
    renderWindow.getSfWindow().setTitle(params.title());
    if (params.vsyncEnabled() != engineSettings.windowParameters().vsyncEnabled()) {
        renderingSystem.vulkanState().swapchain.invalidate();
    }

    engineSettings.withWindowParameters(params);
    params.syncToConfig();

    if (params.letterBox()) {
        sf::Event::SizeEvent e{};
        e.width  = renderWindow.getSfWindow().getSize().x;
        e.height = renderWindow.getSfWindow().getSize().y;
        handleResize(e, false);
    }
    else if (params.syncOverlaySize()) {
        cam::OverlayCamera::setOverlayCoordinateSpace(renderWindow.getSfWindow().getSize().x,
                                                      renderWindow.getSfWindow().getSize().y);
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

    sf::FloatRect viewport(0.f, 0.f, 1.f, 1.f);
    if (engineSettings.windowParameters().letterBox()) {
        const float xScale = newWidth / ogSize.x;
        const float yScale = newHeight / ogSize.y;

        if (xScale >= yScale) { // constrained by height, bars on sides
            windowScale    = yScale;
            viewport.width = ogSize.x * yScale / newWidth;
            viewport.left  = (1.f - viewport.width) * 0.5f;
        }
        else { // constrained by width, bars on top and bottom
            windowScale     = xScale;
            viewport.height = ogSize.y * xScale / newHeight;
            viewport.top    = (1.f - viewport.height) * 0.5f;
        }
    }

    if (renderingSystem.vulkanState().device) {
        renderingSystem.processResize(sf::Rect<std::uint32_t>(newWidth * viewport.left,
                                                              newHeight * viewport.top,
                                                              newWidth * viewport.width,
                                                              newHeight * viewport.height));
        if (engineSettings.windowParameters().syncOverlaySize() &&
            !engineSettings.windowParameters().letterBox()) {
            cam::OverlayCamera::setOverlayCoordinateSpace(newWidth, newHeight);
        }
    }

    if (ss) {
        Settings::WindowParameters params = engineSettings.windowParameters();
        params.withVideoMode(
            sf::VideoMode(resize.width, resize.height, params.videoMode().bitsPerPixel));
        engineSettings.withWindowParameters(params);
        params.syncToConfig();
    }

    bl::event::Dispatcher::dispatch<event::WindowResized>({renderWindow});
}

void Engine::setTimeScale(float s) { timeScale = s; }

float Engine::getTimeScale() const { return timeScale; }

void Engine::resetTimeScale() { timeScale = 1.f; }

void Engine::postStateChange(State::Ptr& prev) {
    engineFlags.clear();
    newState.reset();
    states.top()->activate(*this);
    bl::event::Dispatcher::dispatch<event::StateChange>({states.top(), prev});
    if (renderingSystem.vulkanState().device) { renderingSystem.texturePool().releaseUnused(); }
}

pcl::ParticleSystem& Engine::particleSystem() {
    return ecsSystems.getSystem<pcl::ParticleSystem>();
}

Player& Engine::addPlayer() {
    auto& observer = renderingSystem.addObserver();
    auto& actor    = input.addActor();
    auto& player   = players.emplace_back(Player(*this, &observer, &actor));
    bl::event::Dispatcher::dispatch<event::PlayerAdded>({player});
    return player;
}

void Engine::removePlayer(int i) {
    const unsigned int j = i >= 0 ? i : players.size() - 1;
    auto it              = std::next(players.begin(), j);
    bl::event::Dispatcher::dispatch<event::PlayerRemoved>({*it});
    renderingSystem.removeObserver(j);
    input.removeActor(j);
    players.erase(it);
}

void Engine::renderThreadBody() {
    while (renderThreadShouldRun) {
        std::unique_lock lock(renderingMutex);
        renderingCv.wait(lock);
        if (!renderThreadShouldRun) { break; }

        renderingSystem.renderFrame();
    }
}

} // namespace engine
} // namespace bl
