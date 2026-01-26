#include <BLIB/Interfaces/GUI/Event.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Util/Visitor.hpp>
#include <optional>

namespace bl
{
namespace gui
{
Event Event::fromSFML(const sf::Event& event, const sf::Vector2f& pos) {
    return event.visit(util::Visitor{
        [&pos](const sf::Event::MouseMoved&) { return Event(Event::MouseMoved, pos); },
        [](const sf::Event::MouseLeft&) {
            return Event(Event::MouseOutsideWindow, sf::Vector2f(-1000000.f, -100000000.f));
        },
        [&pos](const sf::Event::TextEntered& e) {
            return Event(Event::TextEntered, e.unicode, pos);
        },
        [&pos](const sf::Event::KeyPressed& ke) { return Event(Event::KeyPressed, ke, pos); },
        [&pos](const sf::Event::KeyReleased& ke) {
            auto kpCopy = sf::Event::KeyPressed(
                ke.code, ke.scancode, ke.alt, ke.control, ke.shift, ke.system);
            return Event(Event::KeyReleased, kpCopy, pos);
        },
        [&pos](const sf::Event::MouseWheelScrolled& e) {
            return Event(Event::Scrolled, e.delta, pos);
        },
        [&pos](const sf::Event::MouseButtonPressed& e) {
            if (e.button == sf::Mouse::Button::Left) { return Event(Event::LeftMousePressed, pos); }
            else if (e.button == sf::Mouse::Button::Right) {
                return Event(Event::RightMousePressed, pos);
            }
            BL_LOG_ERROR << "gui::Event: Unknown mouse button: " << static_cast<int>(e.button);
            return Event(Event::Unknown, pos);
        },
        [&pos](const sf::Event::MouseButtonReleased& e) {
            if (e.button == sf::Mouse::Button::Left) {
                return Event(Event::LeftMouseReleased, pos);
            }
            else if (e.button == sf::Mouse::Button::Right) {
                return Event(Event::RightMouseReleased, pos);
            }
            BL_LOG_ERROR << "gui::Event: Unknown mouse button: " << static_cast<int>(e.button);
            return Event(Event::Unknown, pos);
        },
        [](const auto&) {
            // Handle all other unsupported event types
            return Event(Event::Unknown);
        }});
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

Event::Event(Type type, sf::Event::KeyPressed key, const sf::Vector2f& pos)
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

sf::Event::KeyPressed Event::key() const { return std::get<sf::Event::KeyPressed>(data); }

const sf::Vector2f& Event::dragStart() const { return std::get<sf::Vector2f>(data); }

const std::string& Event::stringValue() const { return std::get<std::string>(data); }

} // namespace gui
} // namespace bl
