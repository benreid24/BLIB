#include <BENG/Menu/Item.hpp>

#include <algorithm>

namespace bg
{
namespace menu
{
Item::Ptr Item::create(RenderItem::Ptr renderItem) { return Ptr(new Item(renderItem)); }

Item::Item(RenderItem::Ptr renderItem)
: renderItem(renderItem)
, attached(false)
, canBeSelected(true)
, allowSelectionCross(false) {}

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

bool Item::isAttached() const { return attached; }

void Item::setSelectable(bool s) { canBeSelected = s; }

bool Item::isSelectable() const { return canBeSelected; }

void Item::setAllowSelectionCrossing(bool a) { allowSelectionCross = a; }

bool Item::allowsSelectionCrossing() const { return allowSelectionCross; }

Signal<>& Item::getSignal(EventType e) { return signals[e]; }

const RenderItem& Item::getRenderItem() const { return *renderItem; }

bool Item::attach(Ptr item, AttachPoint point) {
    if (attachments[point]) return false;
    if (item->attachments[oppositeSide(point)]) return false;

    attachments[point]                     = item;
    item->attachments[oppositeSide(point)] = shared_from_this();
    attached = item->attached = true;
    return true;
}

void Item::visit(Visitor visitor) const {
    std::list<std::pair<int, int>> visited;
    visit(visitor, 0, 0, visited);
}

void Item::visit(Visitor visitor, int x, int y,
                 std::list<std::pair<int, int>>& visited) const {
    if (std::find(visited.begin(), visited.end(), std::make_pair(x, y)) != visited.end())
        return;
    visited.push_back(std::make_pair(x, y));
    visitor(*this, x, y);

    if (attachments[Top]) attachments[Top]->visit(visitor, x, y - 1, visited);
    if (attachments[Right]) attachments[Right]->visit(visitor, x + 1, y, visited);
    if (attachments[Bottom]) attachments[Bottom]->visit(visitor, x, y + 1, visited);
    if (attachments[Left]) attachments[Left]->visit(visitor, x - 1, y, visited);
}

} // namespace menu

} // namespace bg
