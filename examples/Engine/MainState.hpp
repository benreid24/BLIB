#ifndef MAINSTATE_HPP
#define MAINSTATE_HPP

#include "InputListener.hpp"
#include <BLIB/Engine.hpp>
#include <BLIB/Graphics.hpp>

class MainState : public bl::engine::State {
public:
    static Ptr create();

    virtual ~MainState() = default;
    virtual const char* name() const override;

    virtual void activate(bl::engine::Engine& engine) override;
    virtual void deactivate(bl::engine::Engine& engine) override;

    virtual void update(bl::engine::Engine& engine, float dt, float) override;

private:
    bl::ctr::Ref<bl::engine::World> world;
    bl::resource::Ref<sf::VulkanFont> font;
    bl::gfx::Text kbmControls;
    bl::gfx::Text jsControls;
    bl::gfx::Rectangle cover;
    bool inited;
    InputListener listener;

    MainState();
};

#endif
