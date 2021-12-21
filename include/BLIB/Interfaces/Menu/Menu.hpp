#ifndef BLIB_MENU_MENU_HPP
#define BLIB_MENU_MENU_HPP

#include <BLIB/Interfaces/Menu/Event.hpp>
#include <BLIB/Interfaces/Menu/Item.hpp>
#include <BLIB/Interfaces/Menu/Selector.hpp>
#include <BLIB/Util/Hashes.hpp>

#include <list>
#include <unordered_map>

namespace bl
{
/// Collection of classes to create mouseless menus
namespace menu
{
/**
 * @brief Primary class for mouseless menus
 *
 * @ingroup Menu
 *
 */
class Menu {
public:
    /**
     * @brief Create a new Menu with the base Item and selection indicator
     *
     */
    Menu(const Selector::Ptr& selector);

    /**
     * @brief Set the position to render the menu at
     *
     */
    void setPosition(const sf::Vector2f& position);

    /**
     * @brief Render the menu to the given target using the given renderer at the given
     *        position with the given states.
     *
     * @param target The target to render to
     * @param renderStates Render states to use
     */
    void render(sf::RenderTarget& target, sf::RenderStates renderStates = {}) const;

    /**
     * @brief Processes the event and updates the Menu state
     *
     * @param event The event to process
     */
    void processEvent(const Event& event);

    /**
     * @brief Sets the root item of the menu. Clears all other items
     *
     * @param root The new item to be the root
     */
    void setRootItem(const Item::Ptr& root);

    /**
     * @brief Adds a new item to the menu and attaches it to the given parent item
     *
     * @param item The item to add
     * @param parent The item to attach to. Must already be in the menu
     * @param attachPoint The part of the parent item to attach to
     * @param reverse True to attach item back to parent, false for a one way connection
     */
    void addItem(const Item::Ptr& item, Item* parent, Item::AttachPoint attachPoint,
                 bool reverse = true);

    /**
     * @brief Attach two items that are already in the menu. Allows for items to have multiple
     *        connections
     *
     * @param item The item to add. Must already be in the menu
     * @param parent The item to attach to. Must already be in the menu
     * @param attachPoint The part of the parent item to attach to
     * @param reverse True to attach item back to parent, false for a one way connection
     */
    void attachExisting(Item* item, Item* parent, Item::AttachPoint attachPoint,
                        bool reverse = true);

    /**
     * @brief Removes the given item from the menu. Optionally connects surrounding items to each
     *        other horizontally and vertically
     *
     * @param item The item to remove
     * @param connect True to connect surrounding items, false to create a hole
     */
    void removeItem(Item* item, bool connect = true);

    /**
     * @brief Set the selected item. No check is done to ensure that the given item is valid
     *
     * @param item The item to select. Must be in the menu
     */
    void setSelectedItem(Item* item);

    /**
     * @brief Sets the padding to place between elements
     *
     */
    void setPadding(const sf::Vector2f& padding);

    /**
     * @brief Sets the minimum height an item should take
     *
     */
    void setMinHeight(float mh);

    /**
     * @brief Sets the minimum width an item should take
     *
     */
    void setMinWidth(float mw);

    /**
     * @brief Refreshes the positions of all items in the menu
     *
     */
    void refreshPositions();

    /**
     * @brief Returns the bounds of the menu
     *
     */
    const sf::FloatRect& getBounds() const;

private:
    sf::Vector2f position;
    sf::FloatRect bounds;
    std::vector<Item::Ptr> items;
    Selector::Ptr selector;
    Item* selectedItem;
    sf::Vector2f padding;
    sf::Vector2f minSize;

    sf::Vector2f move(const sf::Vector2f& pos, const sf::Vector2f& psize, const sf::Vector2f& esize,
                      Item::AttachPoint ap);
};

} // namespace menu
} // namespace bl

#endif
