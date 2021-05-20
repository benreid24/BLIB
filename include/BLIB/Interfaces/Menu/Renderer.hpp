#ifndef BLIB_MENU_RENDERER_HPP
#define BLIB_MENU_RENDERER_HPP

#include <BLIB/Interfaces/Menu/Item.hpp>

#include <unordered_map>

namespace bl
{
namespace menu
{
/**
 * @brief Base renderer class for Menu renderers
 *
 * @ingroup Menu
 *
 */
class Renderer {
public:
    virtual ~Renderer() = default;

    /**
     * @brief Render the given item with the given guides
     *
     * @param target The target to render to
     * @param renderStates The render states to use
     * @param item The item to render
     * @param position The position of the area to render the item in
     * @param columnWidth The maximum width of any item in this column
     * @param rowHeight The maximum height of any item in this row
     * @param x The row of the item
     * @param y The column of the item
     * @return sf::Vector2f The size of the region the item was rendered to
     */
    virtual sf::Vector2f renderItem(sf::RenderTarget& target, sf::RenderStates renderStates,
                                    const Item& item, const sf::Vector2f& position,
                                    float columnWidth, float rowHeight, int x, int y) const = 0;

    /**
     * @brief Returns the estimated total render area required by the given item
     *
     * @param item The item to estimate size for
     * @return sf::Vector2f The amount of space the item will take
     */
    virtual sf::Vector2f estimateItemSize(const Item& item) const = 0;
};

} // namespace menu
} // namespace bl

#endif
