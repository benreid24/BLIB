#include <BLIB/Interfaces/Menu/Item.hpp>

#include <algorithm>

namespace bl
{
namespace menu
{
Item::Item()
: canBeSelected(true)
, allowSelectionCross(true)
, positionOverridden(false) {
    for (unsigned int i = 0; i < AttachPoint::_NUM_ATTACHPOINTS; ++i) { attachments[i] = nullptr; }
}

Item::AttachPoint Item::oppositeSide(AttachPoint point) {
    switch (point) {
    case Top:
        return Bottom;
    case Right:
        return Left;
    case Bottom:
        return Top;
    case Left:
        return Right;
    default:
        return _NUM_ATTACHPOINTS;
    }
}

void Item::setSelectable(bool s) { canBeSelected = s; }

bool Item::isSelectable() const { return canBeSelected; }

void Item::setAllowSelectionCrossing(bool a) { allowSelectionCross = a; }

bool Item::allowsSelectionCrossing() const { return allowSelectionCross; }

util::Signal<>& Item::getSignal(EventType e) { return signals[e]; }

void Item::overridePosition(const sf::Vector2f& pos) {
    position           = pos;
    positionOverridden = true;
}

} // namespace menu

} // namespace bl
