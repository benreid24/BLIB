#ifndef REBINDSTATE_HPP
#define REBINDSTATE_HPP

#include <BLIB/Engine.hpp>
#include <BLIB/Input.hpp>
#include <BLIB/Resources.hpp>

class RebindState : public bl::engine::State {
public:
    static Ptr create(bl::engine::Engine& engine, unsigned int ctrl);

    virtual ~RebindState() = default;
    virtual const char* name() const override;

    virtual void activate(bl::engine::Engine& engine) override;
    virtual void deactivate(bl::engine::Engine& engine) override;

    virtual void update(bl::engine::Engine& engine, float dt) override;
    virtual void render(bl::engine::Engine& engine, float lag) override;

private:
    bl::input::Configurator configurator;

    bl::resource::Resource<sf::Font>::Ref font;
    sf::Text text;

    RebindState(bl::engine::Engine& engine, unsigned int ctrl);
};

#endif
