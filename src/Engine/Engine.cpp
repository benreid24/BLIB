#include <BLIB/Engine/Engine.hpp>

#include <BLIB/Audio.hpp>
#include <BLIB/Logging.hpp>
#include <BLIB/Particles/ParticleSystem.hpp>
#include <BLIB/Resources/GarbageCollector.hpp>
#include <BLIB/Resources/State.hpp>
#include <BLIB/Signals/Table.hpp>
#include <BLIB/Systems.hpp>
#include <BLIB/Systems/MarkedForDeath.hpp>
#include <BLIB/Util/Visitor.hpp>
#include <SFML/Window.hpp>
#include <cmath>

namespace bl
{
namespace engine
{
namespace
{
float toSeconds(std::uint64_t microseconds) {
    return static_cast<float>(microseconds) / 1'000'000.f;
}

std::uint64_t scaleTime(std::uint64_t microseconds, float scale) {
    return static_cast<std::uint64_t>(static_cast<float>(microseconds) * scale);
}

float toRealSeconds(std::int64_t microseconds, float scale) {
    return static_cast<float>(microseconds) / scale / 1'000'000.f;
}
} // namespace

Engine::Engine(const Settings& settings)
: engineSettings(settings)
, timeScale(1.f)
, ecsSystems(*this)
, entityRegistry()
, renderThreadShouldRun(true)
, input(*this) {
    settings.syncToConfig();

    systems().registerSystem<sys::TogglerSystem>(FrameStage::Update0, StateMask::All);
    systems().registerSystem<sys::VelocitySystem>(FrameStage::Animate,
                                                  StateMask::Running | engine::StateMask::Editor);
    systems().registerSystem<pcl::ParticleSystem>(FrameStage::Update0, StateMask::All);
    systems().registerSystem<sys::MarkedForDeath>(FrameStage::Update0, StateMask::All);
    systems().registerSystem<sys::Physics2D>(FrameStage::Physics, StateMask::Running);

    eventEmitter.connect(signalChannel);
    sig::Table::registerChannel(SignalChannelKey, signalChannel);
    sig::Table::registerChannel(this, signalChannel);

    entityRegistry.getSignalChannel().setParent(signalChannel);
    input.subscribe(signalChannel);
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
    signalChannel.shutdown();
    if (rendererInstance.has_value()) {
        vkCheck(vkDeviceWaitIdle(rendererInstance.value().vulkanState().getDevice()));
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

    if (rendererInstance.has_value()) { rendererInstance->earlyCleanup(); }

    systems().earlyCleanup();
    entityRegistry.destroyAllEntities();

    audio::AudioSystem::shutdown();
    resource::GarbageCollector::get().clear();
    systems().cleanup();

    if (rendererInstance.has_value()) { rendererInstance->cleanup(); }

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
    if (engineSettings.createRenderer()) {
        rendererInstance.emplace(*this, engineSettings.getRendererCreationSettings().value());
        if (!rendererInstance->initialize()) {
            BL_LOG_ERROR << "Failed to initialize renderer";
            return false;
        }
        if (rendererInstance->getSettings().getWindowSettings().letterBox()) {
            sf::Event::Resized e{};
            e.size.x = rendererInstance->getWindow().getSfWindow().getSize().x;
            e.size.y = rendererInstance->getWindow().getSfWindow().getSize().y;
            handleResize(e, false);
        }

        renderingThread.emplace(&Engine::renderThreadBody, this);
    }
    ecsSystems.init();
    return true;
}

bool Engine::loop() {
    BL_LOG_INFO << "Starting engine with state: " << states.top()->name();

    sf::Clock loopTimer;        // measures duration of entire loop top to bottom
    sf::Clock updateOuterTimer; // measure duration from update tick start to start
    const float minFrameLengthSeconds =
        engineSettings.maximumFramerate() > 0.f ? 1.f / engineSettings.maximumFramerate() : 0.f;
    // always limit to at least 100us to avoid issues with SFML time resolution
    const std::uint64_t minFrameLength =
        std::max(static_cast<std::uint64_t>(minFrameLengthSeconds * 1'000'000.f), 100ull);
    std::uint64_t lag = 0;

    sf::Clock updateMeasureTimer; // measures duration of update ticks
    std::uint64_t updateTimestep    = engineSettings.updateTimestepMicroseconds();
    std::uint64_t averageUpdateTime = engineSettings.updateTimestepMicroseconds();

    float lastWarnTime = -6.f; // always log first warning
    bool followupLog   = false;
    auto fallBehindWarning =
        [&lastWarnTime, &updateMeasureTimer, &followupLog](std::uint64_t behind) {
            if (updateMeasureTimer.getElapsedTime().asSeconds() - lastWarnTime >= 5.f) {
                lastWarnTime = updateMeasureTimer.getElapsedTime().asSeconds();
                followupLog  = true;
                BL_LOG_WARN << "Can't catch up, running " << toSeconds(behind) << " seconds behind";
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
    eventEmitter.emit<event::Startup>({states.top()});

    while (true) {
        // Clear flags from last loop
        engineFlags.clear();

        if (rendererInstance.has_value()) {
            std::optional<sf::Event> event = rendererInstance->getWindow().pollEvent();
            while (event.has_value()) {
                eventEmitter.emit<sf::Event>(event.value());

                bool shouldExit = false;
                event->visit(util::Visitor{
                    [this, &shouldExit](const sf::Event::Closed&) {
                        eventEmitter.emit<event::Shutdown>({event::Shutdown::WindowClosed});
                        renderThreadShouldRun = false;
                        renderingCv.notify_one();
                        shouldExit = true;
                    },
                    [this](const sf::Event::Resized& resize) { handleResize(resize, true); },
                    [this, &updateOuterTimer, &loopTimer, &shouldExit](
                        const sf::Event::FocusLost&) {
                        eventEmitter.emit<event::Paused>({});
                        if (!awaitFocus()) {
                            eventEmitter.emit<event::Shutdown>({event::Shutdown::WindowClosed});
                            shouldExit = true;
                        }
                        eventEmitter.emit<event::Resumed>({});
                        updateOuterTimer.restart();
                        loopTimer.restart();
                    },
                    [](const auto&) {
                        // catch all
                    }});
                if (shouldExit) { return true; }
                event = rendererInstance->getWindow().pollEvent();
            }
        }

        ecsSystems.notifyFrameStart();

        // Update and render
        lag += scaleTime(updateOuterTimer.getElapsedTime().asMicroseconds(), timeScale);
        updateOuterTimer.restart();
        const std::uint64_t startingLag = lag;
        std::uint64_t totalDt           = 0;
        updateMeasureTimer.restart();

        // update until caught up
        while (lag >= updateTimestep) {
            // track tick time
            totalDt += updateTimestep;
            lag -= updateTimestep;

            const std::uint64_t updateStart = updateMeasureTimer.getElapsedTime().asMicroseconds();
            const float dt                  = toSeconds(updateTimestep);
            const float realDt              = toRealSeconds(updateTimestep, timeScale);
            const float lagSeconds          = toSeconds(lag);
            const float realLagSeconds      = toRealSeconds(lag, timeScale);

            // core update game logic
            input.update();
            states.top()->update(*this, dt, realDt);
            ecsSystems.update(FrameStage::FrameStart,
                              FrameStage::MARKER_OncePerFrame,
                              states.top()->systemsMask(),
                              dt,
                              realDt,
                              lagSeconds,
                              realLagSeconds);
            signalChannel.syncDeferred();

            // check if we should end early
            if (engineFlags.stateChangeReady()) {
                lag = 0.f;
                break;
            }

            // handle timing
            averageUpdateTime =
                (8 * averageUpdateTime +
                 2 * (updateMeasureTimer.getElapsedTime().asMicroseconds() - updateStart)) /
                10;
            if (updateMeasureTimer.getElapsedTime().asMicroseconds() > startingLag * 11 / 10) {
                fallBehindWarning(updateMeasureTimer.getElapsedTime().asMicroseconds() -
                                  startingLag);
                if (engineSettings.allowVariableTimestep()) {
                    const float newTs = updateTimestep * 21 / 20;
                    BL_LOG_INFO << "Adjusting update timestep from " << updateTimestep << "s to "
                                << newTs << "s";
                    updateTimestep    = newTs;
                    averageUpdateTime = updateTimestep;
                }
                else {
                    skipUpdatesInfo(
                        std::ceil(static_cast<float>(lag) / static_cast<float>(updateTimestep)));
                    lag = 0;
                }
            }
        }

        // update timing
        if (averageUpdateTime < updateTimestep * 9 / 10 &&
            updateTimestep > engineSettings.updateTimestepMicroseconds()) {
            const std::uint64_t newTs =
                std::max(engineSettings.updateTimestepMicroseconds(), updateTimestep * 19 / 20);
            BL_LOG_INFO << "Performance improved, adjusting timestep from "
                        << toSeconds(updateTimestep) << "s to " << toSeconds(newTs) << "s";
            updateTimestep = newTs;
        }

        // wait for render thread to finish current frame & block it
        std::unique_lock renderLock(renderingMutex);

        // Free world slots
        for (auto& world : worlds) {
            if (world && world.refCount() == 1) {
                eventEmitter.emit<event::WorldDestroyed>({*world});
                world.release();
            }
        }

        // Process flags
        bool stateChanged = false;
        while (engineFlags.stateChangeReady()) {
            stateChanged = true;

            if (engineFlags.active(Flags::Terminate)) {
                eventEmitter.emit<event::Shutdown>({event::Shutdown::Terminated});
                return true;
            }
            else if (engineFlags.active(Flags::PopState)) {
                BL_LOG_INFO << "Popping state: " << states.top()->name();
                State::Ptr prev = states.top();
                prev->deactivate(*this);
                states.pop();
                if (states.empty()) { // exit if no states left
                    BL_LOG_INFO << "Final state popped, exiting";
                    eventEmitter.emit<event::Shutdown>({event::Shutdown::FinalStatePopped});
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
            if (rendererInstance.has_value()) {
                // run once per frame systems
                ecsSystems.update(FrameStage::MARKER_OncePerFrame,
                                  FrameStage::COUNT,
                                  states.top()->systemsMask(),
                                  toSeconds(totalDt),
                                  toRealSeconds(totalDt, timeScale),
                                  toSeconds(lag),
                                  toRealSeconds(lag, timeScale));

                // flush scene object changes
                rendererInstance->syncSceneObjects();

                // Copy shared data to renderer while in locked section
                rendererInstance->copyDataFromSources();

                // Flush ECS deletion queues
                entityRegistry.flushDeletions();

                // signal rendering thread to start
                renderLock.unlock();
                renderingCv.notify_one();
            }

            // Adhere to FPS cap
            const std::uint64_t st = minFrameLength - loopTimer.getElapsedTime().asMicroseconds();
            if (st > 0) { sf::sleep(sf::microseconds(st)); }
            loopTimer.restart();

            frameCount += 1.f;
            if (rendererInstance && fpsTimer.getElapsedTime().asSeconds() >= 1.f &&
                engineSettings.logFps()) {
                const float fps = frameCount / fpsTimer.getElapsedTime().asSeconds();
                rendererInstance->getWindow().getSfWindow().setTitle(
                    rendererInstance->getSettings().getWindowSettings().title() + " (" +
                    std::to_string(int(std::roundf(fps))) + " fps)");
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
    while (rendererInstance->getWindow().isOpen()) {
        std::optional<sf::Event> event = rendererInstance->getWindow().waitEvent();
        if (event.has_value()) {
            if (event->is<sf::Event::Closed>()) { return false; }
            if (event->is<sf::Event::FocusGained>()) { return true; }
        }
    }
    return false;
}

void Engine::handleResize(const sf::Event::Resized& resize, bool ss) {
    if (!rendererInstance.has_value()) return;

    if (ss) {
        auto& windowSettings = rendererInstance->getSettings().getWindowSettings();
        windowSettings.withVideoMode(
            sf::VideoMode(resize.size, windowSettings.videoMode().bitsPerPixel));

        // will contain updated windowSettings
        rc::CreationSettings params = rendererInstance->getCreationSettings();
        engineSettings.withRenderer(params);
        engineSettings.syncToConfig();
    }

    eventEmitter.emit<rc::event::WindowResized>({rendererInstance->getWindow()});
}

void Engine::setTimeScale(float s) { timeScale = s; }

float Engine::getTimeScale() const { return timeScale; }

void Engine::resetTimeScale() { timeScale = 1.f; }

void Engine::postStateChange(State::Ptr& prev) {
    engineFlags.clear();
    newState.reset();
    states.top()->activate(*this);
    eventEmitter.emit<event::StateChange>({states.top(), prev});
    if (rendererInstance) { rendererInstance->texturePool().releaseUnused(); }
}

pcl::ParticleSystem& Engine::particleSystem() {
    return ecsSystems.getSystem<pcl::ParticleSystem>();
}

Player& Engine::addPlayer() {
    auto& observer = rendererInstance->addObserver();
    auto& actor    = input.addActor();
    auto& player   = players.emplace_back(new Player(*this, &observer, &actor));
    eventEmitter.emit<event::PlayerAdded>({*player});
    return *player;
}

void Engine::removePlayer(int i) {
    const unsigned int j = i >= 0 ? i : players.size() - 1;
    auto it              = players.begin() + j;
    eventEmitter.emit<event::PlayerRemoved>({**it});
    rendererInstance->removeObserver(j);
    input.removeActor(j);
    players.erase(it);
}

void Engine::renderThreadBody() {
    while (renderThreadShouldRun) {
        std::unique_lock lock(renderingMutex);
        renderingCv.wait(lock);
        if (!renderThreadShouldRun) { break; }

        rendererInstance->renderFrame();
    }
}

} // namespace engine
} // namespace bl
