#ifndef BLIB_MENU_MENU_HPP
#define BLIB_MENU_MENU_HPP

#include <BLIB/Menu/Event.hpp>
#include <BLIB/Menu/Item.hpp>
#include <BLIB/Menu/Renderer.hpp>
#include <BLIB/Util/PairHash.hpp>

#include <list>
#include <unordered_map>

namespace bl
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
    Menu(Item::Ptr root);

    void render(const Renderer& renderer, sf::RenderTarget& target,
                const sf::Vector2f& position, sf::RenderStates renderStates = {});

    void processEvent(const Event& event);

    void refresh();

private:
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
} // namespace bl

#endif
