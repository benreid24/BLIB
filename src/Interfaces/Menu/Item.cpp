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

bool Item::isAttached() const {
    for (unsigned int i = 0; i < AttachPoint::_NUM_ATTACHPOINTS; ++i) {
        if (attachments[i] != nullptr) { return true; }
    }
    return false;
}

void Item::setSelectable(bool s) { canBeSelected = s; }

bool Item::isSelectable() const { return canBeSelected; }

void Item::setAllowSelectionCrossing(bool a) { allowSelectionCross = a; }

bool Item::allowsSelectionCrossing() const { return allowSelectionCross; }

util::Signal<>& Item::getSignal(EventType e) { return signals[e]; }

void Item::overridePosition(const glm::vec2& pos) {
    position           = pos;
    positionOverridden = true;
    updatePosition();
}

const glm::vec2& Item::getPosition() const { return position; }

void Item::notifyPosition(const glm::vec2& p) {
    position = p;
    updatePosition();
}

void Item::notifyOffset(const glm::vec2& o) {
    offset = o;
    updatePosition();
}

void Item::updatePosition() {
    if (transform) { transform->setPosition(position - offset); }
}

void Item::create(engine::Engine& engine, ecs::Entity parent) {
    transform = &doCreate(engine, parent);
}

} // namespace menu
} // namespace bl
