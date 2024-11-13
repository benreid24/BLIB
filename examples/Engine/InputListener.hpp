#ifndef INPUTLISTENER_HPP
#define INPUTLISTENER_HPP

#include <BLIB/Graphics.hpp>
#include <BLIB/Input/Listener.hpp>
#include <BLIB/Render.hpp>
#include <BLIB/Resources.hpp>

class InputListener : public bl::input::Listener {
public:
    InputListener();

    virtual ~InputListener() = default;

    void init(bl::engine::World& world);

    void addToScene(bl::rc::Scene* scene);

    bool shouldRebindExample();

    bool shouldRebindMovement();

private:
    bl::resource::Ref<sf::VulkanFont> font;
    bl::gfx::Text text;
    bool rebindExample;
    bool rebindMovement;

    virtual bool observe(const bl::input::Actor&, unsigned int activatedControl,
                         bl::input::DispatchType eventType, bool eventTriggered) override;
};

#endif
