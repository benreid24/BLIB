#ifndef REBINDSTATE_HPP
#define REBINDSTATE_HPP

#include <BLIB/Engine.hpp>
#include <BLIB/Graphics.hpp>
#include <BLIB/Input.hpp>
#include <BLIB/Resources.hpp>

class RebindState : public bl::engine::State {
public:
    static Ptr create(bl::engine::Engine& engine, unsigned int ctrl);

    virtual ~RebindState() = default;
    virtual const char* name() const override;

    virtual void activate(bl::engine::Engine& engine) override;
    virtual void deactivate(bl::engine::Engine& engine) override;

    virtual void update(bl::engine::Engine& engine, float dt, float) override;
    virtual void render(bl::engine::Engine& engine, float lag) override;

private:
    bl::input::Configurator configurator;

    bl::resource::Ref<sf::VulkanFont> font;
    bl::gfx::Text text;
    const unsigned int ctrl;
    bool inited;

    RebindState(bl::engine::Engine& engine, unsigned int ctrl);
};

#endif
