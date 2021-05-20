#include <BLIB/Interfaces/GUI/RawEvent.hpp>
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

RawEvent::RawEvent(const sf::Event& event, const sf::Vector2f& mpos, const sf::Transform& tform)
: event(event)
, globalMousePos(extractPos(event).value_or(mpos))
, localMousePos(tform.transformPoint(globalMousePos)) {}

RawEvent::RawEvent(const sf::Event& event, const sf::Vector2f& gpos, const sf::Vector2f& lpos)
: event(event)
, globalMousePos(gpos)
, localMousePos(lpos) {}

RawEvent RawEvent::transformToLocal(const sf::Vector2f& parentPos) const {
    return RawEvent(event, globalMousePos, localMousePos - parentPos);
}

} // namespace gui
} // namespace bl