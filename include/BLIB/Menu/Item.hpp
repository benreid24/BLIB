#ifndef BLIB_MENU_ITEM_HPP
#define BLIB_MENU_ITEM_HPP

#include <BLIB/Menu/RenderItem.hpp>
#include <BLIB/Util/Signal.hpp>

#include <functional>
#include <list>
#include <memory>

namespace bl
{
namespace menu
{
class Menu;
class Renderer;

/**
 * @brief Item for a mouseless menu. Can be 'attached' to other items in a Menu,
 *        configured to be selectable, and can trigger a signal if selected
 *
 * @ingroup Menu
 *
 */
class Item : public std::enable_shared_from_this<Item> {
public:
    typedef std::shared_ptr<Item> Ptr;
    typedef std::function<void(const Item&, int, int)> Visitor;
    enum AttachPoint { Top, Right, Bottom, Left, _NUM_ATTACHPOINTS };
    enum EventType { Selected, Deselected, Activated, _NUM_EVENTS };

    /**
     * @brief Calculates the opposite side of the given attach point
     *
     */
    static AttachPoint oppositeSide(AttachPoint point);

    /**
     * @brief Creates a new Item using the given RenderItem
     *
     * @param renderItem The renderable entity for this item
     */
    static Ptr create(RenderItem::Ptr renderItem);

    /**
     * @brief Returns true if the item has been attached to another
     *
     */
    bool isAttached() const;

    /**
     * @brief Set whether or not the Item may be selected
     *
     */
    void setSelectable(bool selectable);

    /**
     * @brief Returns whether or not the Item may be selected
     *
     */
    bool isSelectable() const;

    /**
     * @brief Set whether or not the selection is allowed to move through this item if this
     *        item is not selectable itself
     *
     * @param allow True to allow item on the other side to be selected, false to block
     */
    void setAllowSelectionCrossing(bool allow);

    /**
     * @brief Returns whether or not the selection may move through this item is it is not
     *        selectable itself
     *
     */
    bool allowsSelectionCrossing() const;

    /**
     * @brief Returns a modifiable reference to the Signal object for the corresponding
     *        EventType
     *
     * @param event The EventType the Signal is triggered by
     * @return Signal<>& The modifiable signal
     */
    util::Signal<>& getSignal(EventType event);

    /**
     * @brief Get the RenderItem for this Item
     *
     */
    const RenderItem& getRenderItem() const;

    /**
     * @brief Attach the given item to this item at the attach point. Fails if the given item
     *        is already attached to another or if the given attach point is used.
     *
     * @param item The item to attach
     * @param attachPoint Where to attach the item
     * @return True if the item was attached, false otherwise
     */
    bool attach(Ptr item, AttachPoint attachPoint);

    /**
     * @brief Invokes the visitor with this Item and all attached Items. Visitor is called
     *        recursively to traverse the entire tree. Visitor arguments are (item, x, y),
     *        where x and y are relative to this root item
     *
     * @param visitor The visitor to invoke all attached items on
     */
    void visit(Visitor visitor) const;

private:
    RenderItem::Ptr renderItem;
    Ptr attachments[_NUM_ATTACHPOINTS];
    util::Signal<> signals[_NUM_EVENTS];
    bool canBeSelected;
    bool allowSelectionCross;
    bool attached;

    Item(RenderItem::Ptr renderItem);
    void visit(Visitor visitor, int x, int y, std::list<std::pair<int, int>>& visited) const;

    friend class Menu;
};

} // namespace menu
} // namespace bl

#endif
