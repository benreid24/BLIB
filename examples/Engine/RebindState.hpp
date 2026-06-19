#ifndef REBINDSTATE_HPP
#define REBINDSTATE_HPP

#include <BLIB/Assets/Builtin/FontPayload.hpp>
#include <BLIB/Assets/TypedRef.hpp>
#include <BLIB/Engine.hpp>
#include <BLIB/Graphics.hpp>
#include <BLIB/Input.hpp>

class RebindState : public bl::engine::State {
public:
    static Ptr create(unsigned int ctrl);

    virtual ~RebindState() = default;
    virtual const char* name() const override;

    virtual void activate(bl::engine::Engine& engine) override;
    virtual void deactivate(bl::engine::Engine& engine) override;

    virtual void update(bl::engine::Engine& engine, float dt, float) override;

private:
    bl::input::Configurator configurator;

    bl::as::TypedRef<bl::asi::FontPayload> font;
    bl::gfx::Text text;
    const unsigned int ctrl;
    bool inited;

    RebindState(unsigned int ctrl);
};

#endif
