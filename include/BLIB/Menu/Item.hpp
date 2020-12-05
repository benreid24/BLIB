#ifndef BLIB_MENU_ITEM_HPP
#define BLIB_MENU_ITEM_HPP

#include <BLIB/Menu/RenderItem.hpp>
#include <BLIB/Util/Signal.hpp>

#include <memory>

namespace bl
{
namespace menu
{
class Menu;

/**
 * @brief Item for a mouseless menu. Can be 'attached' to other items in a Menu,
 *        configured to be selectable, and can trigger a signal if selected
 *
 * @ingroup Menu
 *
 */
class Item {
public:
    typedef std::shared_ptr<Item> Ptr;
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
    static Ptr create(const RenderItem& renderItem);

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
     * @brief Returns whether or not the Item is currently selected
     *
     */
    bool isSelected() const;

    /**
     * @brief Returns a modifiable reference to the Signal object for the corresponding
     *        EventType
     *
     * @param event The EventType the Signal is triggered by
     * @return Signal<>& The modifiable signal
     */
    Signal<>& getSignal(EventType event);

    /**
     * @brief Attach the given item to this item at the attach point. Fails if the given item
     *        is already attached to another or if the given attach point is used.
     *
     * @param item The item to attach
     * @param attachPoint Where to attach the item
     * @return True if the item was attached, false otherwise
     */
    bool attach(Ptr item, AttachPoint attachPoint);

private:
    const RenderItem& renderItem;
    Ptr attachments[_NUM_ATTACHPOINTS];
    AttachPoint attachPoint;
    Signal<> signals[_NUM_EVENTS];
    bool canBeSelected;
    bool selected;

    Item(const RenderItem& renderItem);

    friend class Menu;
};

} // namespace menu
} // namespace bl

#endif