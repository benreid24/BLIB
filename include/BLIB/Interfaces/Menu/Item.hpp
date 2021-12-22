#ifndef BLIB_MENU_ITEM_HPP
#define BLIB_MENU_ITEM_HPP

#include <BLIB/Util/Signal.hpp>
#include <SFML/Graphics.hpp>
#include <functional>
#include <list>
#include <memory>

namespace bl
{
namespace menu
{
class Menu;

/**
 * @brief Item for a mouseless menu. Can be 'attached' to other items in a Menu,
 *        configured to be selectable, and can trigger a signal if selected. Subclasses
 *        of this provide specific behaviors
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
     * @brief Set an explicit position for this item. Default behavior is to grid items
     *
     */
    void overridePosition(const sf::Vector2f& position);

    /**
     * @brief Returns a modifiable reference to the Signal object for the corresponding
     *        EventType
     *
     * @param event The EventType the Signal is triggered by
     * @return Signal<>& The modifiable signal
     */
    util::Signal<>& getSignal(EventType event);

protected:
    /**
     * @brief Construct a new Item
     *
     */
    Item();

    /**
     * @brief Return the untransformed size of the object
     *
     * @return sf::Vector2f Size the object will take
     */
    virtual sf::Vector2f getSize() const = 0;

    /**
     * @brief Render the item to the given target
     *
     * @param target The target to render to
     * @param states RenderStates to apply
     * @param position Position to render at
     */
    virtual void render(sf::RenderTarget& target, sf::RenderStates states,
                        const sf::Vector2f& position) const = 0;

private:
    sf::Vector2f position;
    Item* attachments[_NUM_ATTACHPOINTS];
    AttachPoint parent;
    util::Signal<> signals[_NUM_EVENTS];
    bool canBeSelected;
    bool allowSelectionCross;
    bool positionOverridden;

    friend class Menu;
};

} // namespace menu
} // namespace bl

#endif
