#include <BLIB/Interfaces/GUI/Action.hpp>

#include <BLIB/Logging.hpp>

namespace bl
{
namespace gui
{
Action Action::fromRaw(const RawEvent& event) {
    switch (event.event.type) {
    case sf::Event::TextEntered:
        return Action(Action::TextEntered, event.event.text.unicode, event.localMousePos);
    case sf::Event::KeyPressed:
        return Action(Action::KeyPressed, event.event.key, event.localMousePos);
    case sf::Event::KeyReleased:
        return Action(Action::KeyReleased, event.event.key, event.localMousePos);
    case sf::Event::MouseWheelScrolled:
        return Action(Action::Scrolled, event.event.mouseWheelScroll.delta, event.localMousePos);
    default:
        BL_LOG_ERROR << "gui::Action: Unsupported SFML event type: " << event.event.type;
        return Action(Action::Unknown, event.localMousePos);
    }
}

Action::TData::TData(uint32_t i)
: input(i) {}

Action::TData::TData(float s)
: scroll(s) {}

Action::TData::TData(bool v)
: bvalue(v) {}

Action::TData::TData(sf::Event::KeyEvent k)
: key(k) {}

Action::TData::TData(const sf::Vector2f& d)
: dragStart(d) {}

Action::TData::TData(const Action::CustomData& data)
: custom(data) {}

Action::Action(Type type)
: type(type)
, data(0u)
, position() {}

Action::Action(Type type, const sf::Vector2f& pos)
: type(type)
, data(0u)
, position(pos) {}

Action::Action(Type type, uint32_t c, const sf::Vector2f& pos)
: type(type)
, data(c)
, position(pos) {}

Action::Action(Type type, float v)
: type(type)
, data(v) {}

Action::Action(Type type, bool v)
: type(type)
, data(v) {}

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

Action::Action(const Action::CustomData& data, const sf::Vector2f& pos)
: type(Custom)
, data(data)
, position(pos) {}

} // namespace gui
} // namespace bl