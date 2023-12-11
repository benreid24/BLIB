#ifndef BLIB_MENU_ITEM_HPP
#define BLIB_MENU_ITEM_HPP

#include <BLIB/Components/Transform2D.hpp>
#include <BLIB/Render/Overlays/Overlay.hpp>
#include <BLIB/Util/Signal.hpp>
#include <SFML/Graphics.hpp>
#include <functional>
#include <glm/glm.hpp>
#include <list>
#include <memory>

namespace bl
{
namespace engine
{
class Engine;
}

namespace menu
{
class Menu;
class SubmenuItem;

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
     */
    static AttachPoint oppositeSide(AttachPoint point);

    /**
     * @brief Returns true if the item has been attached to another
     */
    bool isAttached() const;

    /**
     * @brief Set whether or not the Item may be selected
     */
    void setSelectable(bool selectable);

    /**
     * @brief Returns whether or not the Item may be selected
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
     */
    bool allowsSelectionCrossing() const;

    /**
     * @brief Set an explicit position for this item. Default behavior is to grid items
     */
    void overridePosition(const glm::vec2& position);

    /**
     * @brief Returns a modifiable reference to the Signal object for the corresponding
     *        EventType
     *
     * @param event The EventType the Signal is triggered by
     * @return Signal<>& The modifiable signal
     */
    util::Signal<>& getSignal(EventType event);

    /**
     * @brief Returns the position of the button relative to it's menu
     */
    const glm::vec2& getPosition() const;

    /**
     * @brief Return the untransformed size of the object
     *
     * @return Size the item will take up
     */
    virtual glm::vec2 getSize() const = 0;

protected:
    /**
     * @brief Construct a new Item
     *
     */
    Item();

    /**
     * @brief Called at least once when the item is added to a menu. Should create required graphics
     *        primitives
     *
     * @param engine The game engine instance
     */
    virtual void doCreate(engine::Engine& engine) = 0;

    /**
     * @brief Called when the item should be added to the overlay
     *
     * @param overlay The overlay to add to
     */
    virtual void doSceneAdd(rc::Overlay* overlay) = 0;

    /**
     * @brief Called when the item should be removed from the overlay
     */
    virtual void doSceneRemove() = 0;

    /**
     * @brief Returns the entity (or top level entity) of the item
     */
    virtual ecs::Entity getEntity() const = 0;

private:
    engine::Engine* enginePtr;
    glm::vec2 position;
    glm::vec2 offset;
    Item* attachments[_NUM_ATTACHPOINTS];
    AttachPoint parent;
    util::Signal<> signals[_NUM_EVENTS];
    bool canBeSelected;
    bool allowSelectionCross;
    bool positionOverridden;
    glm::vec2 posOverride;

    void create(engine::Engine& engine, ecs::Entity parent);
    void notifyPosition(const glm::vec2& position);
    void notifyOffset(const glm::vec2& offset);
    void updatePosition();

    friend class Menu;
    friend class SubmenuItem;
};

} // namespace menu
} // namespace bl

#endif
