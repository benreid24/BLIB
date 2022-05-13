#ifndef BLIB_MENU_MENU_HPP
#define BLIB_MENU_MENU_HPP

#include <BLIB/Interfaces/Menu/Event.hpp>
#include <BLIB/Interfaces/Menu/Item.hpp>
#include <BLIB/Interfaces/Menu/Selector.hpp>
#include <BLIB/Media/Audio/AudioSystem.hpp>
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
     * @brief Sets the background parameters for simple backgrouns. Default is no background
     *
     * @param fill The background color
     * @param outline The outline color around the background and menu
     * @param outlineThickness The thickness of the outline
     * @param padding Padding between menu elements and the edge of the background on each side
     */
    void configureBackground(sf::Color fill, sf::Color outline, float outlineThickness,
                             const sf::FloatRect& padding = {-1.f, -1.f, -1.f, -1.f});

    /**
     * @brief Refreshes the positions of all items in the menu
     *
     */
    void refreshPositions();

    /**
     * @brief Returns the bounds of the menu. Ignores the maximum size
     *
     */
    sf::FloatRect getBounds() const;

    /**
     * @brief Set the maximum size the menu may take up before scrolling. Set negative dimensions to
     *        prevent scrolling in that dimension. Default is no max size
     *
     */
    void setMaximumSize(const sf::Vector2f& maxSize);

    /**
     * @brief Returns the maximum size of the menu
     *
     */
    const sf::Vector2f& maximumSize() const;

    /**
     * @brief Returns the currently visible size of the menu
     *
     */
    sf::Vector2f visibleSize() const;

    /**
     * @brief Returns the current scroll offset of the menu
     *
     */
    const sf::Vector2f& currentOffset() const;

    /**
     * @brief Returns the currently selected item
     *
     */
    const Item* getSelectedItem() const;

    /**
     * @brief Sets the sound that plays when the selector is moved
     *
     * @param sound The sound to play
     */
    void setMoveSound(audio::AudioSystem::Handle sound);

    /**
     * @brief Sets the sound to play when the selector cannot move
     *
     * @param sound The sound to play
     */
    void setMoveFailSound(audio::AudioSystem::Handle sound);

    /**
     * @brief Sets the sound to play when an item is activated
     *
     * @param sound The sound to play
     */
    void setSelectSound(audio::AudioSystem::Handle sound);

    /**
     * @brief Sets the sound that plays when the selector is moved. Applies to all new menus
     *
     * @param sound The sound to play
     */
    static void setDefaultMoveSound(audio::AudioSystem::Handle sound);

    /**
     * @brief Sets the sound to play when the selector cannot move. Applies to all new menus
     *
     * @param sound The sound to play
     */
    static void setDefaultMoveFailSound(audio::AudioSystem::Handle sound);

    /**
     * @brief Sets the sound that plays when the selector is moved. Applies to all new menus
     *
     * @param sound The sound to play
     */
    static void setDefaultSelectSound(audio::AudioSystem::Handle sound);

private:
    sf::Vector2f maxSize;
    sf::Vector2f position;
    sf::Vector2f offset;
    std::vector<Item::Ptr> items;
    Selector::Ptr selector;
    Item* selectedItem;
    sf::Vector2f padding;
    sf::Vector2f minSize;
    sf::RectangleShape background;
    sf::Vector2f totalSize;
    sf::FloatRect bgndPadding;
    audio::AudioSystem::Handle moveSound;
    audio::AudioSystem::Handle failSound;
    audio::AudioSystem::Handle selectSound;

    sf::Vector2f move(const sf::Vector2f& pos, const sf::Vector2f& psize, const sf::Vector2f& esize,
                      Item::AttachPoint ap);
    void refreshScroll();
    void playSound(audio::AudioSystem::Handle sound) const;

    static audio::AudioSystem::Handle defaultMoveSound;
    static audio::AudioSystem::Handle defaultFailSound;
    static audio::AudioSystem::Handle defaultSelectSound;
};

} // namespace menu
} // namespace bl

#endif
