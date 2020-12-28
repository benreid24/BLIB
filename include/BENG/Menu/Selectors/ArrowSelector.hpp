#ifndef BLIB_MENU_SELECTORS_ARROWSELECTOR_HPP
#define BLIB_MENU_SELECTORS_ARROWSELECTOR_HPP

#include <BENG/Menu/Selector.hpp>
#include <BENG/Shapes/Triangle.hpp>

namespace bg
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
     */
    static Ptr create(float width);

    /**
     * @see Selector::render
     *
     */
    virtual void render(sf::RenderTarget& target, sf::RenderStates states,
                        sf::FloatRect itemArea) const override;

private:
    const float width;
    mutable bg::Triangle triangle;

    ArrowSelector(float width);
};

} // namespace menu
} // namespace bg

#endif