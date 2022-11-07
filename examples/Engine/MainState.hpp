#ifndef MAINSTATE_HPP
#define MAINSTATE_HPP

#include "InputListener.hpp"
#include <BLIB/Engine.hpp>

class MainState : public bl::engine::State {
public:
    static Ptr create();

    virtual ~MainState() = default;
    virtual const char* name() const override;

    virtual void activate(bl::engine::Engine& engine) override;
    virtual void deactivate(bl::engine::Engine& engine) override;

    virtual void update(bl::engine::Engine& engine, float dt) override;
    virtual void render(bl::engine::Engine& engine, float lag) override;

private:
    bl::resource::Resource<sf::Font>::Ref font;
    sf::Text kbmControls;
    sf::Text jsControls;
    sf::RectangleShape cover;
    InputListener listener;

    MainState();
};

#endif
