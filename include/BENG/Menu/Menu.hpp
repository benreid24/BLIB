#ifndef BLIB_MENU_MENU_HPP
#define BLIB_MENU_MENU_HPP

#include <BENG/Menu/Event.hpp>
#include <BENG/Menu/Item.hpp>
#include <BENG/Menu/Renderer.hpp>
#include <BENG/Menu/Selector.hpp>
#include <BENG/Util/PairHash.hpp>

#include <list>
#include <unordered_map>

namespace bg
{
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
    Menu(Item::Ptr root, Selector::Ptr selector);

    /**
     * @brief Render the menu to the given target using the given renderer at the given
     *        position with the given states.
     *
     * @param renderer The renderer to use
     * @param target The target to render to
     * @param position The position to render the root element at
     * @param renderStates Render states to use
     */
    void render(const Renderer& renderer, sf::RenderTarget& target,
                const sf::Vector2f& position, sf::RenderStates renderStates = {});

    /**
     * @brief Processes the event and updates the Menu state
     *
     * @param event The event to process
     */
    void processEvent(const Event& event);

    /**
     * @brief Refreshes the calculated sizes of each item. Needs to be called if items
     *        are added or updated after the Menu is constructed
     *
     */
    void refresh();

private:
    Selector::Ptr selector;
    Item::Ptr rootItem;
    Item::Ptr selectedItem;
    std::unordered_map<int, float> columnWidths;
    std::unordered_map<int, float> rowHeights;
    std::vector<std::pair<sf::FloatRect, Item::Ptr>> itemAreas;

    void renderItem(const Renderer& renderer, sf::RenderTarget& target, Item::Ptr item,
                    const sf::Vector2f& position, sf::RenderStates renderStates, int x, int y,
                    std::list<std::pair<int, int>>& rendered);
};

} // namespace menu
} // namespace bg

#endif
