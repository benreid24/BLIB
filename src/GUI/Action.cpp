#include <BLIB/GUI/Action.hpp>

#include <iostream>

namespace bl
{
namespace gui
{
Action Action::fromSFML(const sf::Vector2f& mpos, const sf::Event& event) {
    switch (event.type) {
    case sf::Event::TextEntered:
        return Action(Action::TextEntered, mpos);
    case sf::Event::KeyPressed:
        return Action(Action::KeyPressed, event.key, mpos);
    case sf::Event::KeyReleased:
        return Action(Action::KeyReleased, event.key, mpos);
    case sf::Event::MouseWheelScrolled:
        return Action(Action::Scrolled, event.mouseWheelScroll.delta, mpos);
    default:
        std::cerr << "gui::Action: Unsupported SFML event type: " << event.type << std::endl;
        return Action(Action::Unknown, mpos);
    }
}

Action::TData::TData(uint32_t i)
: input(i) {}

Action::TData::TData(float s)
: scroll(s) {}

Action::TData::TData(sf::Event::KeyEvent k)
: key(k) {}

Action::TData::TData(const sf::Vector2f& d)
: dragStart(d) {}

Action::Action(Type type, const sf::Vector2f& pos)
: type(type)
, data(0u)
, position(pos) {}

Action::Action(Type type, uint32_t c, const sf::Vector2f& pos)
: type(type)
, data(c)
, position(pos) {}

Action::Action(Type type, float s, const sf::Vector2f& pos)
: type(type)
, data(s)
, position(pos) {}

Action::Action(Type type, sf::Event::KeyEvent key, const sf::Vector2f& pos)
: type(type)
, data(key)
, position(pos) {}

Action::Action(Type type, const sf::Vector2f& drag, const sf::Vector2f& pos)
: type(type)
, data(drag)
, position(pos) {}

} // namespace gui
} // namespace bl