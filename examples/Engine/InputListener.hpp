#ifndef INPUTLISTENER_HPP
#define INPUTLISTENER_HPP

#include <BLIB/Input/Listener.hpp>
#include <BLIB/Resources.hpp>
#include <SFML/Graphics.hpp>

class InputListener : public bl::input::Listener {
public:
    InputListener();

    virtual ~InputListener() = default;

    void render(sf::RenderTarget& target);

    bool shouldRebindExample();

    bool shouldRebindMovement();

private:
    bl::resource::Resource<sf::Font>::Ref font;
    sf::Text text;
    bool rebindExample;
    bool rebindMovement;

    virtual bool observe(const bl::input::Actor&, unsigned int activatedControl,
                         bl::input::DispatchType eventType, bool eventTriggered) override;
};

#endif
