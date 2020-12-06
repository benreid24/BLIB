#include <BLIB/Menu/Item.hpp>

namespace bl
{
namespace menu
{
Item::Ptr Item::create(const RenderItem& renderItem) { return Ptr(new Item(renderItem)); }

Item::Item(const RenderItem& renderItem)
: renderItem(renderItem)
, attachPoint(_NUM_ATTACHPOINTS)
, canBeSelected(true)
, selected(false) {}

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

bool Item::isAttached() const { return attachPoint != _NUM_ATTACHPOINTS; }

void Item::setSelectable(bool s) {
    canBeSelected = s;
    if (!canBeSelected) selected = false;
}

bool Item::isSelectable() const { return canBeSelected; }

bool Item::isSelected() const { return selected; }

Signal<>& Item::getSignal(EventType e) { return signals[e]; }

const RenderItem& Item::getRenderItem() const { return renderItem; }

bool Item::attach(Ptr item, AttachPoint point) {
    if (attachments[point]) return false;
    if (item->isAttached()) return false;

    attachments[point] = item;
    item->attachPoint  = oppositeSide(point);
    return true;
}

} // namespace menu

} // namespace bl