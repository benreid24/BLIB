#ifndef BLIB_MENU_ITEMS_SUBMENUITEM_HPP
#define BLIB_MENU_ITEMS_SUBMENUITEM_HPP

#include <BLIB/Interfaces/Menu/Item.hpp>
#include <vector>

namespace bl
{
namespace menu
{
class Menu;

/**
 * @brief Wrapper item that serves as a dropdown menu of sorts. A base item is used to provide the
 *        behavior and appearance of this item. Additional menu items can be added as sub-options
 *        that are added/removed from the parent menu when the submenu is opened and closed.
 *
 * @ingroup Menu
 *
 */
class SubmenuItem : public Item {
public:
    /// @brief Pointer to a submenu object
    using Ptr = std::shared_ptr<SubmenuItem>;

    /**
     * @brief Create a new Submenu menu item. Signals on the base item are never triggered. Use
     *        signals on this item instead
     *
     * @param parent The menu this item will be a part of
     * @param item The base item to provide appearance and behavior for this item
     * @param spawnSide Which direction to add the option items on open
     * @param submenuDirection Which direction options should be added to each other in
     * @return Ptr The new menu item
     */
    static Ptr create(Menu& parent, const Item::Ptr& item, AttachPoint spawnSide,
                      AttachPoint submenuDirection);

    /**
     * @brief Destroy the Submenu Item object
     *
     */
    virtual ~SubmenuItem() = default;

    /**
     * @brief Add a new item to the submenu. Items are shown in the order they are added
     *
     * @param option The item to add
     * @param isBackItem If true then this item, when activated, will close the menu
     */
    void addOption(const Item::Ptr& option, bool isBackItem = false);

    /**
     * @brief Opens the menu if not already open. This is called when the item is interacted with
     *
     */
    void openMenu();

    /**
     * @brief Closes the menu if open. This must be called manually as menus do not have a Back
     *        action as of now.
     *
     */
    void closeMenu();

    /**
     * @brief Returns whether or not the menu is currently open
     *
     * @return True if the menu is open, false otherwise
     */
    constexpr bool isOpen() const;

    /**
     * @brief Returns the size of the underlying item as the size of this item
     */
    virtual glm::vec2 getSize() const override;

protected:
    /**
     * @brief Create a new Submenu menu item. Signals on the base item are never triggered. Use
     *        signals on this item instead
     *
     * @param parent The menu this item will be a part of
     * @param item The base item to provide appearance and behavior for this item
     * @param spawnSide Which direction to add the option items on open
     * @param submenuDirection Which direction options should be added to each other in
     */
    SubmenuItem(Menu& parent, const Item::Ptr& item, AttachPoint spawnSide,
                AttachPoint submenuDirection);

    /**
     * @brief Called at least once when the item is added to a menu. Should create required graphics
     *        primitives and return the transform to use
     *
     * @param engine The game engine instance
     */
    virtual void doCreate(engine::Engine& engine) override;

    /**
     * @brief Called when the item should be added to the overlay
     *
     * @param overlay The overlay to add to
     */
    virtual void doSceneAdd(rc::Overlay* overlay) override;

    /**
     * @brief Called when the item should be removed from the overlay
     */
    virtual void doSceneRemove() override;

    /**
     * @brief Returns the entity (or top level entity) of the item
     */
    virtual ecs::Entity getEntity() const override;

private:
    const AttachPoint openDir;
    const AttachPoint menuDir;
    Menu& parent;
    Item::Ptr self;
    std::vector<Item::Ptr> options;
    bool open;
};

} // namespace menu
} // namespace bl

#endif
