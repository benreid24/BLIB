#ifndef BLIB_MENU_RENDERERS_BASICRENDERER_HPP
#define BLIB_MENU_RENDERERS_BASICRENDERER_HPP

#include <BLIB/Menu/Renderer.hpp>

namespace bl
{
namespace menu
{
/**
 * @brief Basic renderer that renders items with padding and alignment
 *
 * @ingroup Menu
 *
 */
class BasicRenderer : public Renderer {
public:
    enum Alignment { Left, Right, Center, Top = Left, Bottom = Right };

    /**
     * @brief Construct a new Basic Renderer object
     *
     * @param horizontalAlignment How to align items horizontally
     * @param verticalAlignment How to align items vertically
     * @param horizontalPadding Padding to place on either side of items
     * @param verticalPadding Padding to place on top and bottom of items
     */
    BasicRenderer(Alignment horizontalAlignment = Left, Alignment verticalAlignment = Top,
                  float horizontalPadding = 2.f, float verticalPadding = 2.f);

    virtual ~BasicRenderer() = default;

    /**
     * @brief Set the vertical padding
     *
     * @param padding Padding in pixels
     */
    void setVerticalPadding(float padding);

    /**
     * @brief Set the horizontal padding
     *
     * @param padding Padding in pixels
     */
    void setHorizontalPadding(float padding);

    /**
     * @brief Set the horizontal alignment of items
     *
     */
    void setHorizontalAlignment(Alignment align);

    /**
     * @brief Set the vertical alignment of items
     *
     */
    void setVerticalAlignment(Alignment align);

    /**
     * @see Renderer::RenderItem
     */
    virtual sf::Vector2f renderItem(sf::RenderTarget& target, sf::RenderStates renderStates,
                                    const Item& item, const sf::Vector2f& position,
                                    float columnWidth, float rowHeight, int x,
                                    int y) const override;

private:
    Alignment horizontalAlignment;
    Alignment verticalAlignment;
    sf::Vector2f padding;
};

} // namespace menu
} // namespace bl

#endif