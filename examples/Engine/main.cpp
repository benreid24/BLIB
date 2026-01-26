#include "Controls.hpp"
#include "EventListener.hpp"
#include "MainState.hpp"

#include <BLIB/Engine.hpp>
#include <BLIB/Game.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

class EngineExample : public bl::game::Game {
    EventListener listener;

    bool performEarlyStartup(int, char**) override { return true; }

    bl::engine::Settings createStartupParameters() override {
        return bl::engine::Settings().withRenderer(bl::rc::CreationSettings().withWindowSettings(
            bl::rc::WindowSettings()
                .withVideoMode(sf::VideoMode({800, 600}, 32))
                .withStyle(sf::Style::Close | sf::Style::Titlebar | sf::Style::Resize)
                .withTitle("BLIB Engine Example")
                .withLetterBoxOnResize(true)));
    }

    bool completeStartup(bl::engine::Engine& engine) override {
        listener.subscribe(engine.getSignalChannel());
        return true;
    }

    bl::engine::State::Ptr createInitialEngineState() override { return MainState::create(); }

    void startShutdown() override { listener.unsubscribe(); }

    void completeShutdown() override {
        // noop
    }
} globalGameInstance; // this just needs to exist
