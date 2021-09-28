#include <BLIB/Interfaces/GUI/Event.hpp>

#include <BLIB/Logging.hpp>
#include <optional>

namespace bl
{
namespace gui
{
namespace
{
std::optional<sf::Vector2f> extractPos(const sf::Event& event) {
    switch (event.type) {
    case sf::Event::MouseWheelScrolled:
        return sf::Vector2f(event.mouseWheelScroll.x, event.mouseWheelScroll.y);
    case sf::Event::MouseButtonPressed:
    case sf::Event::MouseButtonReleased:
        return sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
    case sf::Event::MouseMoved:
        return sf::Vector2f(event.mouseMove.x, event.mouseMove.y);
    default:
        return {};
    }
}
} // namespace

Event Event::fromSFML(const sf::Event& event, const sf::Vector2f& position) {
    const sf::Vector2f pos = extractPos(event).value_or(position);
    switch (event.type) {
    case sf::Event::MouseMoved:
        return Event(Event::MouseMoved, sf::Vector2f(event.mouseMove.x, event.mouseMove.y));
    case sf::Event::MouseLeft:
        return Event(Event::MouseOutsideWindow, sf::Vector2f(-1000000.f, -100000000.f));
    case sf::Event::TextEntered:
        return Event(Event::TextEntered, event.text.unicode, pos);
    case sf::Event::KeyPressed:
        return Event(Event::KeyPressed, event.key, pos);
    case sf::Event::KeyReleased:
        return Event(Event::KeyReleased, event.key, pos);
    case sf::Event::MouseWheelScrolled:
        return Event(Event::Scrolled, event.mouseWheelScroll.delta, pos);
    case sf::Event::MouseButtonPressed:
        if (event.mouseButton.button == sf::Mouse::Left) {
            return Event(Event::LeftMousePressed, pos);
        }
        else if (event.mouseButton.button == sf::Mouse::Right) {
            return Event(Event::RightMousePressed, pos);
        }
        BL_LOG_ERROR << "gui::Event: Unknown mouse button: " << event.mouseButton.button;
        return Event(Event::Unknown, pos);
    case sf::Event::MouseButtonReleased:
        if (event.mouseButton.button == sf::Mouse::Left) {
            return Event(Event::LeftMouseReleased, pos);
        }
        else if (event.mouseButton.button == sf::Mouse::Right) {
            return Event(Event::RightMouseReleased, pos);
        }
        BL_LOG_ERROR << "gui::Event: Unknown mouse button: " << event.mouseButton.button;
        return Event(Event::Unknown, pos);
    case sf::Event::MouseWheelMoved:
        return Event(Event::Unknown, pos);
    default:
        BL_LOG_ERROR << "gui::Event: Unsupported SFML event type: " << event.type;
        return Event(Event::Unknown, pos);
    }
}

Event::Event(Type type)
: t(type)
, data(0u)
, position() {}

Event::Event(const Event& e, const sf::Vector2f& p)
: t(e.t)
, data(e.data)
, position(p) {}

Event::Event(Type type, const sf::Vector2f& pos)
: t(type)
, data(0u)
, position(pos) {}

Event::Event(Type type, uint32_t c, const sf::Vector2f& pos)
: t(type)
, data(c)
, position(pos) {}

Event::Event(Type type, float v)
: t(type)
, data(v) {}

Event::Event(Type type, bool v)
: t(type)
, data(v) {}

Event::Event(Type type, float s, const sf::Vector2f& pos)
: t(type)
, data(s)
, position(pos) {}

Event::Event(Type type, sf::Event::KeyEvent key, const sf::Vector2f& pos)
: t(type)
, data(key)
, position(pos) {}

Event::Event(Type type, const sf::Vector2f& drag, const sf::Vector2f& pos)
: t(type)
, data(drag)
, position(pos) {}

Event::Event(Type type, const std::string& s)
: t(type)
, data(s) {}

Event::Type Event::type() const { return t; }

const sf::Vector2f& Event::mousePosition() const { return position; }

std::uint32_t Event::character() const { return std::get<std::uint32_t>(data); }

float Event::scrollDelta() const { return std::get<float>(data); }

float Event::inputValue() const { return std::get<float>(data); }

bool Event::toggleValue() const { return std::get<bool>(data); }

const sf::Event::KeyEvent& Event::key() const { return std::get<sf::Event::KeyEvent>(data); }

const sf::Vector2f& Event::dragStart() const { return std::get<sf::Vector2f>(data); }

const std::string& Event::stringValue() const { return std::get<std::string>(data); }

} // namespace gui
} // namespace bl
