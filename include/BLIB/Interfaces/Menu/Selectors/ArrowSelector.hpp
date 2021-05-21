#ifndef BLIB_MENU_SELECTORS_ARROWSELECTOR_HPP
#define BLIB_MENU_SELECTORS_ARROWSELECTOR_HPP

#include <BLIB/Interfaces/Menu/Selector.hpp>
#include <BLIB/Media/Shapes/Triangle.hpp>

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
     */
    static Ptr create(float width);

    /**
     * @brief Exposes the underlying triangle
     *
     */
    shapes::Triangle& getArrow();

    /**
     * @see Selector::render
     *
     */
    virtual void render(sf::RenderTarget& target, sf::RenderStates states,
                        sf::FloatRect itemArea) const override;

private:
    const float width;
    mutable bl::shapes::Triangle triangle;

    ArrowSelector(float width);
};

} // namespace menu
} // namespace bl

#endif