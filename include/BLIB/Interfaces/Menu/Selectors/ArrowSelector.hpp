#ifndef BLIB_MENU_SELECTORS_ARROWSELECTOR_HPP
#define BLIB_MENU_SELECTORS_ARROWSELECTOR_HPP

#include <BLIB/Graphics/Triangle.hpp>
#include <BLIB/Interfaces/Menu/Selector.hpp>

namespace bl
{
namespace menu
{
/**
 * @brief Basic selection indicator, draws a triangle to the left of the selected item
 *
 * @ingroup Menu
 *
 */
class ArrowSelector : public Selector {
public:
    typedef std::shared_ptr<ArrowSelector> Ptr;

    /**
     * @brief Create a new ArrowSelector with the given width
     *
     * @param width The width of the arrow in pixels
     * @param fill Color to fill the arrow with
     *
     */
    static Ptr create(float width, const sf::Color& fill = sf::Color::White);

    /**
     * @brief Exposes the underlying triangle
     *
     */
    gfx::Triangle& getArrow();

    /**
     * @see Selector::render
     *
     */
    virtual void render(sf::RenderTarget& target, sf::RenderStates states,
                        sf::FloatRect itemArea) const override;

private:
    const float width;
    mutable gfx::Triangle triangle;

    ArrowSelector(float width, const sf::Color& f);
};

} // namespace menu
} // namespace bl

#endif
